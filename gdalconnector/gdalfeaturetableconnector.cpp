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
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "gdaltableloader.h"

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
    if(!GdalConnector::loadMetaData(data))
        return false;

    int layer = 0;
    OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), layer);
    if ( hLayer){
        GdalTableLoader loader;
        loader.loadMetaData((Table *)data, hLayer);
    }
    return true;
}

bool GdalFeatureTableConnector::storeMetaData(Ilwis::IlwisObject *obj){
    return true;
}

bool GdalFeatureTableConnector::loadBinaryData(IlwisObject * data){
    Table *attTable = static_cast<Table *>(data);

    OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), 0);
    if ( hLayer) {
        std::vector<QVariant> record(attTable->columnCount());

        GdalTableLoader loader;
        loader.setColumnCallbacks(attTable, hLayer);
        OGRFeatureH hFeature = 0;
        gdal()->resetReading(hLayer);
        //each FEATURE
        try {
            while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL){
                loader.loadRecord(attTable, hFeature, record);
                gdal()->destroyFeature( hFeature );
            }
             _binaryIsLoaded = true;
            return true;
        } catch (FeatureCreationError& ) {
            gdal()->destroyFeature( hFeature );
            return false;
        }
    }
    return false;

}



bool GdalFeatureTableConnector::storeBinaryData(IlwisObject* obj){
    return true;
}
