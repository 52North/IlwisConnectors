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
#include "wfsparsingcontext.h"
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

void WfsFeatureParser::parseFeatureMembers(WfsParsingContext &context)
{
    QString targetNamespace =  context.namespaceMappings()[""];
    _parser->addNamespaceMapping("target", targetNamespace);
    ITable table = _fcoverage->attributeTable();
    QString featureType = table->name();
    featureType = "target:" + featureType;

    quint64 featureCount = 0;
    if (_parser->moveToNext("wfs:FeatureCollection")) {
        if (_parser->moveToNext("gml:featureMembers")) {
            while (_parser->moveToNext(featureType)) {
                std::vector<QVariant> record(table->columnCount());
                parseFeature(record, context);
                table->record(featureCount++, record); // load content
            }
        }
        while (_parser->moveToNext("wfs:featureMember")) {

        }
    }
}

void WfsFeatureParser::parseFeature(std::vector<QVariant> &record, WfsParsingContext &context)
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
        if (currentElementName == context.geometryAtttributeName()) {
            parseFeatureGeometry(context);
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
        if (_parser->name() == context.geometryAtttributeName()) {
            parseFeatureGeometry(context);
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

void WfsFeatureParser::parseFeatureGeometry(WfsParsingContext &context)
{
    _parser->readNextStartElement();
    IlwisTypes types = _fcoverage->ilwisType();

    updateSrsInfo(context);
    bool isMultiGeometry = _parser->name().contains("Multi");
    if (types & itPOLYGON) {
        if (isMultiGeometry) {
            std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
            if (updateSrsInfoUntil("gml:surfaceMember", context)) {
                do {
                    updateSrsInfo(context);
                    geos::geom::Polygon *polygon;
                    if ( !parsePolygon(polygon, context)) {
                        ERROR0("Could not parse GML MultiSurface member.");
                        break;
                    } else {
                        multi->push_back(polygon);
                    }
                } while (_parser->moveToNext("gml:surfaceMember"));
                _fcoverage->newFeature(_fcoverage->geomfactory()->createMultiPolygon(multi),false);
            }
        } else {
            updateSrsInfo(context);
            geos::geom::Polygon *polygon;
            if ( !parsePolygon(polygon, context)) {
                ERROR0("Could not parse GML Surface.");
            } else {
                _fcoverage->newFeature(polygon,false);
            }
        }

        // TODO create crs?
        // CoordinateSystem cs = new CoordinateSystem();

    } else if (types & itLINE) {
        if (isMultiGeometry) {
            std::vector<geos::geom::Geometry *>* multi = new std::vector<geos::geom::Geometry *>();
            if (updateSrsInfoUntil("gml:curveMember", context)) {
                do {
                    updateSrsInfo(context);
                    geos::geom::Geometry *geometry;
                    if ( !parseLineString(geometry, context)) {
                        ERROR0("Could not parse GML MultiCurve member.");
                        break;
                    } else {
                        multi->push_back(geometry);
                    }
                } while (_parser->moveToNext("gml:curveMember"));
                _fcoverage->newFeature(_fcoverage->geomfactory()->createMultiLineString(multi),false);
            }
        } else {
            geos::geom::LineString *lineString;
            if ( !parseLineString(lineString, context)) {
                ERROR0("Could not parse GML Curve.");
            }
            _fcoverage->newFeature(_fcoverage->geomfactory()->createLineString(lineString->getCoordinates()),false);
        }

        // TODO create crs?
        // CoordinateSystem cs = new CoordinateSystem();

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

void WfsFeatureParser::updateSrsInfo(WfsParsingContext &context)
{
    QXmlStreamAttributes attributes = _parser->attributes();
    QString srs = attributes.value("srsName").toString();
    QString dimension = attributes.value("srsDimension").toString();
    if (!dimension.isEmpty()) context.setSrsDimension(dimension.toInt());
    if (!srs.isEmpty()) context.setSrsName(srs);
}

bool WfsFeatureParser::updateSrsInfoUntil(QString qname, WfsParsingContext &context)
{
    while (!_parser->isAtBeginningOf(qname)) {
        if (_parser->readNextStartElement()) {
            updateSrsInfo(context);
        } else {
            return false;
        }
    }
    return true;
}

bool WfsFeatureParser::parseLineString(geos::geom::Geometry *lineString, WfsParsingContext &context)
{
    try {
        // move to actual curve member
        _parser->readNextStartElement();
        if (_parser->findNextOf( {"gml:posList, gml:pos"} )) {
            QString wkt = gmlPosListToWktLineString(_parser->readElementText(), context);
            geos::geom::Geometry *geometry = GeometryHelper::fromWKT(wkt.toStdString());
            lineString = _fcoverage->geomfactory()->createLineString(geometry->getCoordinates());
            return true;
        }
        // we only parse gml:posList and gml:pos
        return false;
    } catch(std::exception &e) {
        ERROR1("Could not parse WKT LineString %1", e.what());
        return false;
    }
}

bool WfsFeatureParser::parsePolygon(geos::geom::Geometry *polygon, WfsParsingContext &context)
{
    try {
        geos::geom::LinearRing *outer = parseExteriorRing(context);
        std::vector<geos::geom::Geometry *> *inners = parseInteriorRings(context);
        polygon = _fcoverage->geomfactory()->createPolygon(outer, inners);
        return true;
    } catch(std::exception &e) {
        ERROR1("Could not parse WKT Polygon %1", e.what());
        return false;
    }
}

geos::geom::LinearRing *WfsFeatureParser::parseExteriorRing(WfsParsingContext &context)
{
    geos::geom::LinearRing *ring;

     // move to actual surface member
    _parser->readNextStartElement();
    if (_parser->moveToNext("gml:exterior")) {
        if (_parser->findNextOf( {"gml:posList"} )) {
            QString wkt = gmlPosListToWktPolygon(_parser->readElementText(), context);
            geos::geom::Geometry *geometry = GeometryHelper::fromWKT(wkt.toStdString());
            ring = _fcoverage->geomfactory()->createLinearRing(geometry->getCoordinates());
        }
    }
    return ring;
}


std::vector<geos::geom::Geometry *> *WfsFeatureParser::parseInteriorRings(WfsParsingContext &context)
{
   std::vector<geos::geom::Geometry *>* innerRings = new std::vector<geos::geom::Geometry *>();

   // move to actual surface member
  _parser->readNextStartElement();
    while (_parser->moveToNext("gml:interior")) {
        if (_parser->findNextOf( {"gml:posList" })) {
            QString wkt = gmlPosListToWktPolygon(_parser->readElementText(), context);
            geos::geom::Geometry *geometry = GeometryHelper::fromWKT(wkt.toStdString());
            geos::geom::LinearRing *ring = _fcoverage->geomfactory()->createLinearRing(geometry->getCoordinates());
            innerRings->push_back(ring);
        }
    }
    return innerRings;
}

QString WfsFeatureParser::gmlPosListToWktCoords(QString gmlPosList, WfsParsingContext &context)
{
    QString wktCoords;
    QStringList coords = gmlPosList.split(" ");
    if (context.srsDimension() == 2) {
        for (int i = 0; i < coords.size(); i++) {
            wktCoords.append(coords.at(i++)).append(" ");
            wktCoords.append(coords.at(i)).append(" ");
            if (context.srsDimension() == 3) {
                wktCoords.append(coords.at(++i));
            }
            wktCoords.append(", ");
        }
    }
    return wktCoords.left(wktCoords.lastIndexOf(","));
}

QString WfsFeatureParser::gmlPosListToWktPolygon(QString gmlPosList, WfsParsingContext &context)
{
    QString wkt("POLYGON((");
    wkt.append(gmlPosListToWktCoords(gmlPosList, context));
    return wkt.append("))");
}

QString WfsFeatureParser::gmlPosListToWktLineString(QString gmlPosList, WfsParsingContext &context)
{
    QString wkt("LINESTRING(");
    wkt.append(gmlPosListToWktCoords(gmlPosList, context));
    return wkt.append(")");
}

QString WfsFeatureParser::gmlPosListToWktPoint(QString gmlPosList, WfsParsingContext &context)
{
    QString wkt("POINT(");
    wkt.append(gmlPosListToWktCoords(gmlPosList, context));
    return wkt.append(")");
}

