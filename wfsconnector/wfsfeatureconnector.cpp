#include <QSqlQuery>
#include <QSqlError>
#include <fstream>
#include <iterator>

#include "kernel.h"
#include "coverage.h"
#include "module.h"
#include "coverage.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "table.h"
#include "geometry.h"
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
    featureUrl.setQuery(source().urlQuery());
    WebFeatureService wfs(featureUrl);

    // TODO: load Feature metadata and fill coverage

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IlwisTypes coverageType = itUNKNOWN;
    int featureCount = 0;
    BoundingBox bbox;
    bool initMinMax = 0;

    ITable attTable;
    Resource resource(QUrl(QString("ilwis://internalcatalog/%1_%2").arg(fcoverage->name()).arg(fcoverage->id())), itFLATTABLE);
    if(!attTable.prepare(resource)) {//only internalTableconnector is used! own class not needed
        ERROR1(ERR_NO_INITIALIZED_1,resource.name());
        return false;
    }
    IDomain dmKey;
    dmKey.prepare("count");
    ColumnDefinition colKey(FEATUREIDCOLUMN, dmKey, 0);
    attTable->addColumn(colKey);
    ColumnDefinition colCovKey(COVERAGEKEYCOLUMN, dmKey, 1);
    attTable->addColumn(colCovKey);

    // TODO: Fill attribute/domains via DescribeFeatureType

    return false;
}


bool Ilwis::Wfs::WfsFeatureConnector::store(IlwisObject *obj, int)
{
    // transactional not supported by this module
    return false;
}


bool Ilwis::Wfs::WfsFeatureConnector::loadBinaryData(IlwisObject *data)
{

    // TODO: request data and load it into *data

    return false;
}
