//#include "ilwisdata.h"
//#include "feature.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "gdalproxy.h"
#include "gdalconnector.h"
#include "gdalfeatureconnector.h"

using namespace Ilwis;
using namespace Gdal;

GdalFeatureConnector::GdalFeatureConnector(const Resource &resource, bool load) : GdalConnector(resource,load){
}

ConnectorInterface* GdalFeatureConnector::create(const Resource &resource, bool load) const{
    return new GdalFeatureConnector(resource, load);
}

Ilwis::IlwisObject *RasterCoverageConnector::create() const{
    return new Feature(_resource);
}

bool GdalFeatureConnector::loadMetaData(Ilwis::IlwisObject *data)
{
    if(!GdalConnector::loadMetaData(data))
        return false;

//    Coverage *coverage = static_cast<Coverage *>(data);

//    ICoordinateSystem csy = setObject<ICoordinateSystem>("coordinatesystem", _filename);
//    if(!csy.isValid()) {
//        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "coordinatesystem", coverage->name());
//    }



//    double geosys[6];
//    CPLErr err = gdal()->getGeotransform(_dataSet, geosys) ;
//    if ( err != CE_None) {
//        return ERROR2(ERR_INVALID_PROPERTY_FOR_2, "Bounds", coverage->name());
//    }

//    double a1 = geosys[0];
//    double b1 = geosys[3];
//    double a2 = geosys[1];
//    double b2 = geosys[5];
//    Pixel pix(gdal()->xsize(_dataSet), gdal()->ysize(_dataSet));
//    Coordinate crdLeftup( a1 , b1);
//    Coordinate crdRightDown(a1 + pix.x() * a2, b1 + pix.y() * b2 ) ;
//    Coordinate cMin( min(crdLeftup.x(), crdRightDown.x()), min(crdLeftup.y(), crdRightDown.y()));
//    Coordinate cMax( max(crdLeftup.x(), crdRightDown.x()), max(crdLeftup.y(), crdRightDown.y()));

//    coverage->envelope(Box2D<double>(cMin, cMax));

//    return true;

    return false;
}

bool GdalFeatureConnector::store(IlwisObject *obj, IlwisTypes type): GdalConnector::store(obj, type)
{
}

bool GdalFeatureConnector::setSRS(Coverage *raster, GDALDatasetH dataset) const
{
//    IConventionalCoordinateSystem csy = raster->coordinateSystem().get<ConventionalCoordinateSystem>();
//    QString proj4def = csy->projection()->toProj4();
//    OGRSpatialReferenceH srsH = gdal()->newSRS(0);
//    OGRErr errOgr = gdal()->importFromProj4(srsH, proj4def.toLocal8Bit());
//    if ( errOgr != OGRERR_NONE) {
//        return reportError(dataset);
//    }
//    char *wktText = NULL;
//    gdal()->exportToWkt(srsH,&wktText);
//    CPLErr err = gdal()->setProjection(dataset, wktText);
//    gdal()->free(wktText);
//    if ( err != CP_NONE) {
//        return reportError(dataset);
//    }
//    return true;
    return false;
}
