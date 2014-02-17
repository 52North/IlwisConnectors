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


WfsFeatureDescriptionParser::WfsFeatureDescriptionParser(WfsResponse *response)
{
    _parser = new XmlStreamParser(response->device());
    _parser->addNamespaceMapping("xsd", "http://www.w3.org/2001/XMLSchema");
}

WfsFeatureDescriptionParser::~WfsFeatureDescriptionParser()
{
}

void WfsFeatureDescriptionParser::parseSchemaDescription(ITable &table, QMap<QString,QString> &namespaceMappings) const
{
    if ( !table.isValid()) {
        ERROR0(TR("Invalid table (uninitialized?) while parsing WFS feature description."));
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
            break; // TODO: consider how parse further namespaces
        } /*else {
            QString prefix = attribute.prefix().toString();
            QString parsedPrefix = name.left(name.indexOf(":"));
            namespaceMappings[prefix] = value;
        }*/
    }
}

void WfsFeatureDescriptionParser::parseFeatureProperties(ITable &table) const
{
    if (_parser->moveToNext("xsd:complexContent")) {
        if (_parser->moveToNext("xsd:extension")) {
            if (_parser->moveToNext("xsd:sequence")) {
                if (_parser->moveToNext("xsd:element")) {

                    // TODO: parse schema elements to table colummns to obtain feature
                    //       specific knowledge, e.g. namespaces

                    table->addColumn(FEATUREIDCOLUMN, "count");
                    table->addColumn(COVERAGEKEYCOLUMN, "count");

                    do {
                        QXmlStreamAttributes attributes = _parser->attributes();
                        QString name = attributes.value("name").toString();
                        QString type = attributes.value("type").toString();

                        IDomain domain;
                        setDomainViaType(type, domain);
                        table->addColumn(name, domain);
                    } while (_parser->moveToNext("xsd:element"));
                }
            }
        }
    }

}

void WfsFeatureDescriptionParser::setDomainViaType(QString &type, IDomain &domain) const
{
    if (type == "xsd:double") {
        INumericDomain ndomain;
        ndomain.prepare("value");
        domain = ndomain;
    } else if (type == "xsd:integer") {
        INumericDomain ndomain;
        ndomain.prepare("integer");
        domain = ndomain;
    } else if (type == "xsd:long") {
        INumericDomain ndomain;
        ndomain.prepare("integer");
        domain = ndomain;
    } else if (type == "xsd:string") {
        domain.prepare("code=domain:text", itTEXTDOMAIN);
    } else if (type.startsWith("gml")) {

        // TODO: check for gml spatial types

    }

    // TODO: add more types here

    else {
        ERROR1(TR("Could not create domain for schema type: %1"), type);
    }
}
