#include <QUrl>
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
#include "featureiterator.h"

#include "wfsresponse.h"
#include "xmlstreamparser.h"
#include "wfsfeaturedescriptionparser.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeatureDescriptionParser::WfsFeatureDescriptionParser()
{
}


WfsFeatureDescriptionParser::WfsFeatureDescriptionParser(WfsResponse *response, FeatureCoverage *fcoverage): _fcoverage(fcoverage)
{
    _parser = new XmlStreamParser(response->device());
    _parser->addNamespaceMapping("xsd", "http://www.w3.org/2001/XMLSchema");
}

WfsFeatureDescriptionParser::~WfsFeatureDescriptionParser()
{
}

void WfsFeatureDescriptionParser::parseSchemaDescription(ITable &table, QMap<QString,QString> &namespaceMappings) const
{
    QString name(_fcoverage->name());
    quint64 id = _fcoverage->id();
    QUrl schemaResourceUrl(QString("ilwis://internalcatalog/%1_%2").arg(name).arg(id));

    Resource resource(schemaResourceUrl, itFLATTABLE);
    if(!table.prepare(resource)) {
        ERROR1(ERR_NO_INITIALIZED_1, resource.name());
        return;
    }

    if (_parser->startParsing("xsd:schema")) {
        parseNamespaces(namespaceMappings);
        while ( !_parser->atEnd()) {
            if (_parser->readNextStartElement()) {
                if (_parser->isAtBeginningOf("xsd:complexType")) {
                    parseFeatureProperties(table);
                } else if (_parser->isAtBeginningOf("xsd:element")) {
                    QStringRef typeName = _parser->attributes().value("name");
                    table->setName(typeName.toString());
                }
            }
        }
    }
}

void WfsFeatureDescriptionParser::parseNamespaces(QMap<QString,QString> &namespaceMappings) const
{
    for (QXmlStreamAttribute attribute : _parser->attributes()) {
        QString name = attribute.name().toString();
        QString value = attribute.value().toString();
        if (name == "targetNamespace") {
            namespaceMappings[""] = value;
            break; // TODO: consider further namespaces parsing
        }
    }
}

void WfsFeatureDescriptionParser::parseFeatureProperties(ITable &table) const
{
    if (_parser->nextLevelMoveTo("xsd:complexContent")) {
        if (_parser->nextLevelMoveTo("xsd:extension")) {
            if (_parser->nextLevelMoveTo("xsd:sequence")) {
                if (_parser->nextLevelMoveTo("xsd:element")) {
                    do {

                        // TODO: parse schema elements to table colummns to obtain feature
                        //       specific knowledge, e.g. namespaces

                        dmKey.prepare("count");
                        ColumnDefinition colKey(FEATUREIDCOLUMN, dmKey, 0);
                        table->addColumn(colKey);
                        ColumnDefinition colCovKey(COVERAGEKEYCOLUMN, dmKey, 1);
                        table->addColumn(colCovKey);


                    } while (_parser->currentLevelMoveTo("xsd:element"));
                }
            }
        }
    }

}
