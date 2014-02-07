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

void WfsFeatureParser::setNamespaceMappings(QMap<QString, QString> &mappings)
{
    QMapIterator<QString,QString> iterator(mappings);
    while(iterator.hasNext()) {
        iterator.next();
        _parser->addNamespaceMapping(iterator.key(), iterator.value());
    }
}

void WfsFeatureParser::parseFeature(ITable &table)
{
    // TODO: parser feature item and add property to named table column
    //table->column("ogi:quad100_id")
}
