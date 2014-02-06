#include <QSqlQuery>
#include <QSqlError>
#include <fstream>
#include <iterator>

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
#include "wfsfeatureconnector.h"
#include "wfsfeatureparser.h"
#include "wfsfeaturedescriptionparser.h"
#include "wfs.h"
#include "wfsresponse.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeatureConnector::WfsFeatureConnector(const Resource &resource, bool load) : WfsConnector(resource,load) {
}

ConnectorInterface* WfsFeatureConnector::create(const Resource &resource, bool load) {
    return new WfsFeatureConnector(resource, load);
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

    ITable featureTable;
    QUrlQuery queryFeatureType(featureUrl);
    WfsResponse *featureDescriptionResponse = wfs.describeFeatureType(queryFeatureType);


    // TODO: will this type of resource table creation work? => name, url, query .... all meshed together here :(
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    QUrl schemaResourceUrl(QString("ilwis://internalcatalog/%1_%2").arg(fcoverage->name()).arg(fcoverage->id()));
    WfsFeatureDescriptionParser schemaParser(featureDescriptionResponse, schemaResourceUrl);
    schemaParser.parseSchemaDescription(featureTable);

    QUrlQuery queryFeature(featureUrl);
    WfsResponse *featureResponse = wfs.getFeature(queryFeature);
    WfsFeatureParser featureParser(featureResponse, featureTable);

    // TODO: parse Feature metadata and fill coverage

    IlwisTypes coverageType = itUNKNOWN;
    int featureCount = 0;
    BoundingBox bbox;
    bool initMinMax = 0;


    return false;
}

bool WfsFeatureConnector::store(IlwisObject *obj, int)
{
    // transactional not supported by this module
    return false;
}


bool WfsFeatureConnector::loadBinaryData(IlwisObject *data)
{

    // TODO: request data and load it into *data

    return false;
}
