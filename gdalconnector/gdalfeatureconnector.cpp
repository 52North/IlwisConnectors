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
#include "identifieritem.h"
#include "identifierrange.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"
#include "coverageconnector.h"
#include "gdalfeatureconnector.h"

using namespace Ilwis;
using namespace Gdal;

GdalFeatureConnector::GdalFeatureConnector(const Resource &resource, bool load) : CoverageConnector(resource,load){
}

ConnectorInterface* GdalFeatureConnector::create(const Resource &resource, bool load) {
    return new GdalFeatureConnector(resource, load);
}

Ilwis::IlwisObject* GdalFeatureConnector::create() const{
    return new FeatureCoverage(this->_resource);
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

bool GdalFeatureConnector::store(IlwisObject *obj, IlwisTypes type)
{
    return CoverageConnector::store(obj, type);
}
