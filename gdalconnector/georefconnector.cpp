#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QDir>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "domain.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "gdalproxy.h"
#include "gdalconnector.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "boostext.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "georefconnector.h"


using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *GeorefConnector::create(const Ilwis::Resource &resource,bool load) {
    return new GeorefConnector(resource,load);
}

GeorefConnector::GeorefConnector(const Ilwis::Resource &resource, bool load) : GdalConnector(resource, load)
{
}

bool GeorefConnector::loadMetaData(IlwisObject *data){
    if(!GdalConnector::loadMetaData(data))
        return false;

    //TODO: tiepoints grf

    GeoReference * grf = static_cast<GeoReference *>(data);

    ICoordinateSystem csy = setObject<ICoordinateSystem>("coordinatesystem", _filename.toString());
    if(!csy.isValid()) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "coordinatesystem", grf->name());
    }
    grf->coordinateSystem(csy);

    if(_handle->type() == GdalHandle::etGDALDatasetH){
        QSize sz(gdal()->xsize(_handle->handle()), gdal()->ysize(_handle->handle()));
        double geosys[6];
        CPLErr err = gdal()->getGeotransform(_handle->handle(), geosys) ;
        if ( err != CE_None) {
            return ERROR2(ERR_INVALID_PROPERTY_FOR_2, "Georeference", grf->name());
        }

        double a1 = geosys[0];
        double b1 = geosys[3];
        double a2 = geosys[1];
        double b2 = geosys[5];
        Coordinate crdLeftup( a1 , b1);
        Coordinate crdRightDown(a1 + sz.width() * a2, b1 + sz.height() * b2 ) ;
        Coordinate cMin( min(crdLeftup.x(), crdRightDown.x()), min(crdLeftup.y(), crdRightDown.y()));
        Coordinate cMax( max(crdLeftup.x(), crdRightDown.x()), max(crdLeftup.y(), crdRightDown.y()));

        grf->impl<CornersGeoReference>()->setEnvelope(Box2D<double>(cMin, cMax));
    }else{
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2, "Georeference", grf->name());
    }

    return true;
}

IlwisObject *GeorefConnector::create() const{
    //TODO: tiepoints georef
    return GeoReference::create("corners", Resource());



}


