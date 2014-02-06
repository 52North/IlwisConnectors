#include <QUrl>

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
#include "wfsfeaturedescriptionparser.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeatureDescriptionParser::WfsFeatureDescriptionParser()
{
}


WfsFeatureDescriptionParser::WfsFeatureDescriptionParser(WfsResponse *response, QUrl &url): _url(url)
{
    _parser = new XmlStreamParser(response->device());
}

WfsFeatureDescriptionParser::~WfsFeatureDescriptionParser()
{
}

void WfsFeatureDescriptionParser::parseSchemaDescription(ITable &table)
{
    Resource resource(_url, itFLATTABLE);
    if(!table.prepare(resource)) { //will load whole meta data of the table
        ERROR1(ERR_NO_INITIALIZED_1,resource.name());
        return;
    }

    // TODO: parse schema elements to table colummns to obtain feature
    //       specific knowledge, e.g. namespaces

    IDomain dmKey;
    dmKey.prepare("count");
    ColumnDefinition colKey(FEATUREIDCOLUMN, dmKey, 0);
    table->addColumn(colKey);
    ColumnDefinition colCovKey(COVERAGEKEYCOLUMN, dmKey, 1);
    table->addColumn(colCovKey);

}
