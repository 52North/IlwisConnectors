#include <QSqlQuery>
#include <QSqlError>
#include <fstream>
#include <iterator>

#include "geos/geom/LinearRing.h"

#include "kernel.h"
#include "module.h"
#include "coverage.h"
#include "ilwiscontext.h"
#include "catalog.h"
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
#include "ilwisobjectconnector.h"
#include "wfsconnector.h"
#include "wfsparsingcontext.h"
#include "wfsfeatureconnector.h"
#include "wfsfeatureparser.h"
#include "wfsfeaturedescriptionparser.h"
#include "wfs.h"
#include "wfsresponse.h"
#include "wfsutils.h"

using namespace Ilwis;
using namespace Wfs;

ConnectorInterface* WfsFeatureConnector::create(const Resource &resource, bool load, const PrepareOptions &options) {
    return new WfsFeatureConnector(resource, load);
}

WfsFeatureConnector::WfsFeatureConnector(const Resource &resource, bool load, const Ilwis::PrepareOptions &options) : WfsConnector(resource,load) {
}


Ilwis::IlwisObject* WfsFeatureConnector::create() const {
    return new FeatureCoverage(this->_resource);
}

bool WfsFeatureConnector::loadMetaData(Ilwis::IlwisObject *data)
{
    if (!WfsConnector::loadMetaData(data)) {
        return false;
    }

    QUrl featureUrl = source().url();
    WebFeatureService wfs(featureUrl);
    QUrlQuery queryFeatureType(featureUrl);
    WfsResponse *featureDescriptionResponse = wfs.describeFeatureType(queryFeatureType);
    WfsFeatureDescriptionParser schemaParser(featureDescriptionResponse);
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);

    return schemaParser.parseSchemaDescription(fcoverage, _context);
}

void WfsFeatureConnector::initFeatureTable(ITable &table) const
{

}


bool WfsFeatureConnector::loadBinaryData(IlwisObject *data)
{
    // check how to avoid double loading metadata
    //if(!loadMetaData(data))
    //    return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    QUrl featureUrl = source().url();
    WebFeatureService wfs(featureUrl);

    QUrlQuery queryFeature(featureUrl);
    WfsResponse *response = wfs.getFeature(queryFeature);
    WfsFeatureParser featureParser(response, fcoverage);

    featureParser.context(_context);
    featureParser.parseFeatureMembers();

    return true;
}

IlwisTypes WfsFeatureConnector::ilwisType(const QString &resourceUrl)
{
    QUrl url(resourceUrl);
    if (WfsUtils::isValidWfsUrl(url)) {
        return itUNKNOWN;
    }

    QUrlQuery query(url);
    WfsUtils::lowerCaseKeys(query);
    QString request = query.queryItemValue("request");

    // TODO when stand-alone table connector is present!
    //if (request == "DescribeFeature") {
    //    return itTABLE;
    //}

    if (request == "GetFeature" || request == "DescribeFeature") {
        return itFEATURE;
    }

    return itUNKNOWN;
}
