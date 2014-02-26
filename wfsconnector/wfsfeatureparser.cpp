#include <QMap>
#include <QString>
#include <QXmlStreamAttributes>

#include "geos/io/ParseException.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"

#include "kernel.h"
#include "coverage.h"
#include "ilwis.h"
#include "ilwisdata.h"
#include "domain.h"
#include "datadefinition.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "table.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "textdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "geometryhelper.h"

#include "wfsresponse.h"
#include "xmlstreamparser.h"
#include "wfsschemainfo.h"
#include "wfsfeatureparser.h"
#include "wfsutils.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeatureParser::WfsFeatureParser(WfsResponse *response, FeatureCoverage *fcoverage): _fcoverage(fcoverage)
{
    _parser = new XmlStreamParser(response->device());
    _parser->addNamespaceMapping("wfs", "http://www.opengis.net/wfs");
    _parser->addNamespaceMapping("ows", "http://www.opengis.net/ows");
    _parser->addNamespaceMapping("gml", "http://www.opengis.net/gml");
}

WfsFeatureParser::~WfsFeatureParser()
{
    delete _parser;
}

void WfsFeatureParser::parseFeatureMembers(WfsSchemaInfo &wfsSchemaInfo)
{
    QString targetNamespace =  wfsSchemaInfo.namespaceMappings()[""];
    _parser->addNamespaceMapping("target", targetNamespace);
    ITable table = _fcoverage->attributeTable();
    QString featureType = table->name();
    featureType = "target:" + featureType;

    quint64 featureCount = 0;
    if (_parser->moveToNext("wfs:FeatureCollection")) {
        if (_parser->moveToNext("gml:featureMembers")) {
            while (_parser->moveToNext(featureType)) {
                std::vector<QVariant> record(table->columnCount());
                parseFeature(record, wfsSchemaInfo.geometryAtttributeName());
                table->record(featureCount++, record); // load content
            }
        }
        while (_parser->moveToNext("wfs:featureMember")) {

        }
    }
}

void WfsFeatureParser::parseFeature(std::vector<QVariant> &record, QString geomAttributeName)
{
    bool continueReadingStream = true;
    ITable table = _fcoverage->attributeTable();
    for (int i = 0; i < table->columnCount(); i++) {

        ColumnDefinition& coldef = table->columndefinition(i);
        if ( coldef.name() == QString(FEATUREIDCOLUMN) ) {
            continue; // auto filled column
        }

        DataDefinition& datadef = coldef.datadef();
        if( !datadef.domain().isValid()) {
            WARN2(ERR_NO_INITIALIZED_2, "domain", coldef.name());
            record[i] = QVariant(); // empty
            continue;
        }

        if (continueReadingStream) {
            if ( !_parser->readNextStartElement()) {
                break; // end of feature record
            }
        } else {
            // continue next time
            continueReadingStream = true;
        }

        QString currentElementName = _parser->name();
        if (_parser->isAtBeginningOf("gml:boundedBy")) {

            // TODO: add boundedBy fill function

            _parser->skipCurrentElement(); // ignore for now

            i--; // not written to table
            continue;
        }

        // if geometry is in beteen
        if (currentElementName == geomAttributeName) {
            parseFeatureGeometry();
            i--; // not written to table
            continue;
        }

        // check for missing (optional) elements
        if (coldef.name() != currentElementName) {
            record[i] = QVariant(); // empty
            continueReadingStream = false;
            continue; // pause stream
        }

        IlwisTypes tp = datadef.domain()->valueType();

        if (tp & itSTRING){
            record[i] = fillStringColumn();
        } else if (tp & itINTEGER){
            NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
            //creating the actual range as invalid to be adjusted in the fillers
            double min = r->min();
            r->min(r->max());
            r->max(min);
            datadef.range(r);
            record[i] = fillIntegerColumn();
        } else if (tp & itDOUBLE){
            //creating the actual range as invalid to be adjusted in the fillers
            NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
            double min = r->min();
            r->min(r->max());
            r->max(min);
            datadef.range(r);
            record[i] = fillDoubleColumn();
        } else if (tp & itTIME){
            //creating the actual range as invalid to be adjusted in the fillers
            NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
            double min = r->min();
            r->min(r->max());
            r->max(min);
            datadef.range(r);
            record[i] = fillDateTimeColumn();
        } else {
            record[i] = QVariant();
        }
    }

    if (_parser->readNextStartElement()) {
        if (_parser->name() == geomAttributeName) {
            parseFeatureGeometry();
        }
    }

}



QVariant WfsFeatureParser::fillStringColumn()
{
    return QVariant(_parser->readElementText());
}

QVariant WfsFeatureParser::fillIntegerColumn()
{
    return QVariant(_parser->readElementText().toInt());
}

QVariant WfsFeatureParser::fillDoubleColumn()
{
    return QVariant(_parser->readElementText().toDouble());
}

QVariant WfsFeatureParser::fillDateTimeColumn()
{
    QVariant v = QVariant(_parser->readElementText());

    Time time;

    // TODO: parse xsd:time

    return v;
}

QVariant WfsFeatureParser::parseFeatureGeometry()
{
    QString srsName;
    QString srsDimension;
    _parser->readNextStartElement();
    updateSrsInfo(srsName,srsDimension);
    IlwisTypes types = _fcoverage->ilwisType();

    bool isMultiGeometry = _parser->name().contains("Multi");
    if (types & itPOLYGON) {

        if (isMultiGeometry) {

            std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
            if (updateSrsInfoUntil("gml:surfaceMember", srsName, srsDimension)) {
                do {


                    // use a context object for parsing
                    updateSrsInfo(srsName, srsDimension);



                    multi->push_back(parsePolygon());
                } while (_parser->moveToNext("gml:surfaceMember"));
            }
        } else {

            // TODO: parse single surface

        }

        //CoordinateSystem cs = new CoordinateSystem();

    } else if (types & itLINE) {

    } else if (types & itPOINT) {

    } else {
        // TODO: unknown geometry type

        // we have to react on types present on the xml stream
//        if (_parser->findNextOf({ "gml:GeometryPropertyType",
//                                "gml:MultiSurfaceType",
//                                "gml:MultiCurveType",
//                                "gml:MultiPointType",
//                                "gml:PolygonType",
//                                "gml:CurveType",
//                                "gml:LineStringType",
//                                "gml:PointType",
//                                "gml:RingType",
//                                "gml:LinearRingType"} )) {

//            if (isPolygonType()) {
//                _coverageType |= itPOLYGON;
//            } else if (isLineType()) {
//                _coverageType |= itLINE;
//            } else if (isPointType()) {
//                _coverageType |= itPOINT;
//            }

//        }
    }

    //_fcoverage->newFeature(geos:Geom);
}

void WfsFeatureParser::updateSrsInfo(QString &srsName, QString &srsDimension)
{
    QXmlStreamAttributes attributes = _parser->attributes();
    QString srs = attributes.value("srsName").toString();
    QString dimension = attributes.value("srsDimension").toString();
    srsDimension = dimension.isEmpty() ? srsDimension : dimension;
    srsName = srs.isEmpty() ? srsName : srs;
}

bool WfsFeatureParser::updateSrsInfoUntil(QString qname, QString &srsName, QString srsDimension)
{
    while (!_parser->isAtBeginningOf(qname)) {
        if (_parser->readNextStartElement()) {
            updateSrsInfo(srsName, srsDimension);
        } else {
            return false;
        }
    }
    return true;
}

geos::geom::Geometry *WfsFeatureParser::parsePolygon()
{
    geos::geom::LinearRing *outer = parseExteriorRing();
    std::vector<geos::geom::Geometry *> *inners = parseInteriorRings();
    return _fcoverage->geomfactory()->createPolygon(outer, inners);
}

geos::geom::LinearRing *WfsFeatureParser::parseExteriorRing()
{
    if (_parser->moveToNext("gml:exterior")) {
        if (_parser->moveToNext("gml:posList")) {
            std::string wkt = _parser->readElementText().toStdString();
            geos::geom::Geometry *geometry = GeometryHelper::fromWKT(wkt);
            return _fcoverage->geomfactory()->createLinearRing(geometry->getCoordinates());
        }
    }
}


std::vector<geos::geom::Geometry *> *WfsFeatureParser::parseInteriorRings()
{
   std::vector<geos::geom::Geometry *>* innerRings = new std::vector<geos::geom::Geometry *>();
    while (_parser->moveToNext("gml:interior")) {
        if (_parser->moveToNext("gml:posList")) {
            std::string wkt = _parser->readElementText().toStdString();
            geos::geom::Geometry *geometry = GeometryHelper::fromWKT(wkt);
            geos::geom::LinearRing *ring = _fcoverage->geomfactory()->createLinearRing(geometry->getCoordinates());
            innerRings->push_back(ring);
        }
    }
    return innerRings;
}
