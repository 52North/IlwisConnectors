#include "kernel.h"
#include "ilwisdata.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"
#include "gdalfeaturetableconnector.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"

using namespace Ilwis;
using namespace Gdal;

GdalFeatureTableConnector::GdalFeatureTableConnector(const Ilwis::Resource &resource, bool load) : GdalConnector(resource, load){}

ConnectorInterface *GdalFeatureTableConnector::create(const Resource &resource, bool load) {
    return new GdalFeatureTableConnector(resource, load);

}

IlwisObject* GdalFeatureTableConnector::create() const{
    return new FlatTable(_resource);
}


bool GdalFeatureTableConnector::loadMetaData(IlwisObject* data){
    return false;
}

bool GdalFeatureTableConnector::storeMetaData(Ilwis::IlwisObject *obj){
    return false;
}

bool GdalFeatureTableConnector::loadBinaryData(IlwisObject *){
    return false;
}

bool GdalFeatureTableConnector::storeBinaryData(IlwisObject* obj){
    return false;
}
