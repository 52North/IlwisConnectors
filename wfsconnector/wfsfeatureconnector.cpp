#include <QSqlQuery>
#include <QSqlError>
#include <fstream>
#include <iterator>

#include "kernel.h"
#include "coverage.h"
#include "module.h"
#include "coverage.h"
#include "polygon.h"
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

bool WfsFeatureConnector::loadMetaData(Ilwis::IlwisObject *data){


}


bool Ilwis::Wfs::WfsFeatureConnector::store(IlwisObject *obj, int)
{
    return false;
}


bool Ilwis::Wfs::WfsFeatureConnector::loadBinaryData(IlwisObject *data)
{
    return false;
}
