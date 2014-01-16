#include "kernel.h"
#include "ilwisdata.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"
#include "gdalfeaturetableconnector.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "coverage.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"

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
    return true;
}

bool GdalFeatureTableConnector::storeMetaData(Ilwis::IlwisObject *obj){
    return true;
}

bool GdalFeatureTableConnector::loadBinaryData(IlwisObject * data){
    Coverage* cov = static_cast<Coverage*>(data->parent());
    if ((cov != 0) && cov->isValid()){
        IFeatureCoverage fc;
        fc.prepare(cov->id());
        FeatureIterator it(fc);//TODO this is only a hack to call cov->connector()->loadBinartData(cov)
    }
    return true;// && cov->connector()->loadBinaryData(cov);
}

bool GdalFeatureTableConnector::storeBinaryData(IlwisObject* obj){
    return true;
}
