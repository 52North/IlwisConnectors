#include <QMap>
#include <QString>

#include "kernel.h"
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

#include "wfsresponse.h"
#include "xmlstreamparser.h"
#include "wfsfeatureparser.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeatureParser::WfsFeatureParser(WfsResponse *response)
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

void WfsFeatureParser::parseFeature(ITable &table, QMap<QString, QString> &mappings)
{
    QString featureType = table->name();
    QString targetNamespace = mappings[""];
    _parser->addNamespaceMapping("target", targetNamespace);
    featureType = "target:" + featureType;


    quint64 featureCount = 0;
    if (_parser->moveToNext("wfs:FeatureCollection")) {
        if (_parser->moveToNext("gml:featureMembers")) {
            while (_parser->moveToNext(featureType)) {
                quint64 cIdx = 0;
                std::vector<QVariant> record(table->columnCount());

                setColumnCallbacks(table);
                loadRecord(table, record);

                while (_parser->readNextStartElement()) {
                    QString attribute = _parser->name();
                    ColumnDefinition definition = table->columndefinition(attribute);
                    DataDefinition dataDef = definition.datadef();
                    IDomain domain = dataDef.domain();

                }
                table->record(featureCount++, record);
            }
        }
        while (_parser->moveToNext("wfs:featureMember")) {

        }
    }

    // TODO: parser feature item and add property to named table column

    table->column("ogi:quad100_id");
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
        if( !datadef.domain().isValid()){
            WARN2(ERR_NO_INITIALIZED_2, "domain", coldef.name());
            continue;
        }

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
}

void WfsFeatureParser::loadRecord(ITable &table, std::vector<QVariant>& record )
{
    if (_columnFillers.size() > 0) {
        for (int i = 0; i < table->columnCount();i++){
            if (_columnFillers[i]){
                record[i] = (this->*_columnFillers[i]->fillFunc)();
            }else{
                record[i] = QVariant();
            }
        }
    }
}

QVariant WfsFeatureParser::fillStringColumn()
{
    QVariant v = QVariant(_parser->readElementText());
    _parser->readNextStartElement();
    return v;
}

QVariant WfsFeatureParser::fillIntegerColumn()
{
    QVariant v = QVariant(_parser->readElementText().toInt());
    _parser->readNextStartElement();
    return v;
}

QVariant WfsFeatureParser::fillDoubleColumn()
{
    QVariant v = QVariant(_parser->readElementText().toDouble());
    _parser->readNextStartElement();
    return v;
}

QVariant WfsFeatureParser::fillDateTimeColumn()
{
    Time time;

    // TODO: parse xsd:time

    QVariant v = QVariant(_parser->readElementText());
    _parser->readNextStartElement();
    return v;
}
