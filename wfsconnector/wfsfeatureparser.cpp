#include <QMap>
#include <QString>
#include <QXmlStreamAttributes>

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

#include "wfsresponse.h"
#include "xmlstreamparser.h"
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

    _columnFillers.resize(0);
}

WfsFeatureParser::~WfsFeatureParser()
{
    delete _parser;
    for(FillerColumnDef *def : _columnFillers)
        delete def;
}

void WfsFeatureParser::parseFeatureMembers(QMap<QString, QString> &mappings)
{
    ITable table = _fcoverage->attributeTable();
    QString featureType = table->name();
    QString targetNamespace = mappings[""];
    _parser->addNamespaceMapping("target", targetNamespace);
    featureType = "target:" + featureType;

    quint64 featureCount = 0;
    if (_parser->moveToNext("wfs:FeatureCollection")) {
        if (_parser->moveToNext("gml:featureMembers")) {
            setColumnCallbacks(table);
            while (_parser->moveToNext(featureType)) {
                std::vector<QVariant> record(table->columnCount());
                loadRecord(table, record);
                table->record(featureCount++, record);
            }
        }
        while (_parser->moveToNext("wfs:featureMember")) {

        }
    }

    // TODO: parser feature item and add property to named table column

}


void WfsFeatureParser::setColumnCallbacks(ITable &table)
{

    _columnFillers.resize(table->columnCount(),0);
    for (int i = 0; i < table->columnCount(); i++){
        ColumnDefinition& coldef = table->columndefinition(i);
        if ( coldef.name() == QString(FEATUREIDCOLUMN) ) {
            continue; // auto filled column
        }

        DataDefinition& datadef = coldef.datadef();
        if( !datadef.domain().isValid()) {
            WARN2(ERR_NO_INITIALIZED_2, "domain", coldef.name());
            continue;
        }

        // TODO: add boundedBy fill function


        IlwisTypes tp = datadef.domain()->valueType();
        if (tp & itSTRING){
            _columnFillers[i] = new FillerColumnDef(&WfsFeatureParser::fillStringColumn);
        } else if (tp & itINTEGER){
            NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
            //creating the actual range as invalid to be adjusted in the fillers
            double min = r->min();
            r->min(r->max());
            r->max(min);
            datadef.range(r);
            _columnFillers[i] = new FillerColumnDef(&WfsFeatureParser::fillIntegerColumn);
        } else if (tp & itDOUBLE){
            //creating the actual range as invalid to be adjusted in the fillers
            NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
            double min = r->min();
            r->min(r->max());
            r->max(min);
            datadef.range(r);
            _columnFillers[i] = new FillerColumnDef(&WfsFeatureParser::fillDoubleColumn);
        } else if (tp & itTIME){
            //creating the actual range as invalid to be adjusted in the fillers
            NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
            double min = r->min();
            r->min(r->max());
            r->max(min);
            datadef.range(r);
            _columnFillers[i] = new FillerColumnDef(&WfsFeatureParser::fillDateTimeColumn);
        }
    }

    /*
     * Geometry can be defined anywhere in the xml sequence. We add the geometry
     * parsing function at the end, so we have a well defined index for it.
     */
    FillerColumnDef *geometryFiller = new FillerColumnDef(&WfsFeatureParser::parseFeatureGeometry);
    _columnFillers[_columnFillers.size()] = geometryFiller;
}

void WfsFeatureParser::loadRecord(ITable &table, std::vector<QVariant>& record )
{
    if (_columnFillers.size() > 0) {
        for (int i = 0; i < table->columnCount();i++) {
            if (_parser->readNextStartElement()) {
                QString name = _parser->name();
                if (name == GEOM_ATTRIBUTE_NAME) {
                    // geometry parsing function has well defined index
                    size_t geomParserFuncIdx = _columnFillers.size() - 1;
                    (this->*_columnFillers[geomParserFuncIdx]->fillFunc)();
                    i--; // keep iteration index
                    continue;
                }

                if (_columnFillers[i]){
                    record[i] = (this->*_columnFillers[i]->fillFunc)();
                } else{
                    record[i] = QVariant();
                }
            }
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
    _parser->readNextStartElement();
    IlwisTypes types = _fcoverage->ilwisType();
    if (types & itPOLYGON) {
        while (_parser->moveToNext("gml:exterior")) {

            QXmlStreamAttributes attributes = _parser->attributes();
            QString srs = attributes.value("gml:srsName").toString();
            srsName = srs.isEmpty() ? "" : srs;

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
