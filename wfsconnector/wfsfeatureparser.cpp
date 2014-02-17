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
}

WfsFeatureParser::~WfsFeatureParser()
{
    delete _parser;
}

void WfsFeatureParser::parseFeature(ITable &table, QMap<QString, QString> &mappings) const
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
                std::vector<QVariant> records(table->columnCount());
                while (_parser->readNextStartElement()) {
                    QString attribute = _parser->name();
                    ColumnDefinition definition = table->columndefinition(attribute);
                    DataDefinition dataDef = definition.datadef();
                    IDomain domain = dataDef.domain();
                }
                table->record(featureCount++, records);
            }
        }
        while (_parser->moveToNext("wfs:featureMember")) {

        }
    }

    // TODO: parser feature item and add property to named table column

    table->column("ogi:quad100_id");
}

void WfsFeatureParser::parseFeatureAttribute(quint64 id, QString attribute) const
{
//    std::vector<QVariant> record(table->columnCount());

}
