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

IlwisTypes GdalFeatureConnector::translateOGRType(OGRwkbGeometryType type) const{
    IlwisTypes ret = itUNKNOWN;

    if ( type == wkbPoint || type == wkbMultiPoint || type == wkbPoint25D || type == wkbMultiPoint25D)
        ret += itPOINT;

    if ( type == wkbLineString || type == wkbMultiLineString || type == wkbLineString25D || type == wkbMultiLineString25D)
        ret += itLINE;

    if ( type == wkbPolygon || type == wkbMultiPolygon || type == wkbPolygon25D || type == wkbMultiPolygon25D)
        ret += itPOLYGON;

    return ret;
}


bool GdalFeatureConnector::loadMetaData(Ilwis::IlwisObject *data){

    if(!CoverageConnector::loadMetaData(data))
        return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IlwisTypes coverageType = itUNKNOWN;
    int featureCount = 0;
    Box2D<double> bbox;
    bool initMinMax = 0;

    for(int layer = 0; layer < gdal()->getLayerCount(_handle->handle()) ; ++layer) {
        OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), layer);
        if ( hLayer) {

            //feature types
            IlwisTypes type = translateOGRType(gdal()->getLayerGeometry(hLayer));
            if (type == itUNKNOWN){
                ERROR2(ERR_COULD_NOT_LOAD_2,QString("layer from: %1").arg(_filename),QString(":%1").arg(gdal()->getLastErrorMsg()));
            }
            coverageType |= type;

            //feature counts
            int temp = gdal()->getFeatureCount(hLayer, FALSE);//TRUE to FORCE databases to scan whole layer, FALSe can end up in -1 for unknown result
            featureCount = fcoverage->featureCount(type);
            featureCount += (temp == -1) ? 0 : temp;
            fcoverage->setFeatureCount(type, featureCount);

            //layer envelopes/extents
            OGREnvelope envelope;//might sometimes be supported as 3D now only posssible from OGRGeometry
            OGRErr er = gdal()->getLayerExtent(hLayer, &envelope , FALSE);//TRUE to FORCE
            if (er ==  OGRERR_FAILURE){
                ERROR2(ERR_COULD_NOT_LOAD_2,QString("(TRY FORCE) extent of a layer from: %2").arg(_filename),QString(":%1").arg(gdal()->getLastErrorMsg()));
            }
            if(!initMinMax){
                bbox=Box2D<double>(Coordinate2d(envelope.MinX,envelope.MinY),Coordinate2d(envelope.MaxX,envelope.MaxY));
            }else{
                if(bbox.max_corner().x() < envelope.MaxX)
                    bbox.max_corner().x(envelope.MaxX);
                if(bbox.max_corner().y() < envelope.MaxY)
                    bbox.max_corner().y(envelope.MaxY);
                if(bbox.min_corner().x() > envelope.MinX)
                    bbox.min_corner().x(envelope.MinX);
                if(bbox.min_corner().y() > envelope.MinY)
                    bbox.min_corner().y(envelope.MinY);
            }
        }
    }
    if (coverageType != itUNKNOWN && featureCount >= 0){
        fcoverage->featureTypes(coverageType);
        fcoverage->envelope(bbox);
    }else{
       return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Records",data->name());
    }

//    ITable tbl = fcoverage->attributeTable();
//    tbl->setRows(fcoverage->featureCount());

    return true;
}

bool GdalFeatureConnector::loadBinaryData(IlwisObject* data){
    if ( data == nullptr)
        return false;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    if ( fcoverage->isValid() ) {
        for(int layer = 0; layer < gdal()->getLayerCount(_handle->handle()) ; ++layer) {
            OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), layer);
            if ( hLayer) {
                OGRFeatureDefnH hFeatureDef = gdal()->getLayerDef(hLayer);
                if ( hFeatureDef) {
//                    Table tbl;
//                    createTable(fnBaseOutputName, dm, hFeatureDef, hLayer, tbl);
//                    if ( tbl.fValid()) {
//                        bmp->SetAttributeTable(tbl);
//                    }
                    int rec = 1;
                    OGRFeatureH hFeature;
                    gdal()->resetReading(hLayer);
                    while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL ){
                        GeometryType geom = fillFeature(gdal()->getGeometryRef(hFeature), rec);
//                        SPFeatureI feature = fcoverage->newFeature({geom});
                        gdal()->destroyFeature( hFeature );
                    }
                }
            }
        }
    }
    return true;
}

GeometryType GdalFeatureConnector::fillFeature(OGRGeometryH geometry, int& rec) const{
    if (geometry){
        switch (translateOGRType(gdal()->getGeometryType(geometry))){
            case itPOINT:   return fillPoint(geometry, rec);    break;
            case itLINE:    return fillLine(geometry, rec);     break;
            case itPOLYGON: return fillPolygon(geometry, rec);  break;
            default:
                ERROR2(ERR_INVALID_PROPERTY_FOR_2, "GeometryType of a Feature", _filename);
                break;
        }
    }
    return nullptr;
}
GeometryType GdalFeatureConnector::fillPoint(OGRGeometryH geometry, int& rec) const{
    double x,y,z;
    gdal()->getPoints(geometry, 0,&x,&y,&z);
    Coordinate coord(std::vector<double>({x,y,z}));
    rec++;
    return coord;
}
GeometryType GdalFeatureConnector::fillLine(OGRGeometryH geometry, int& rec) const{
    return nullptr;
}

GeometryType GdalFeatureConnector::fillPolygon(OGRGeometryH geometry, int &rec) const{
    long count = gdal()->getSubGeometryCount(geometry);
    for(int i = 0; i < count; ++i) {
        OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
        fillPolygon(hSubGeometry, rec);
    }
    Polygon p;
    return p;
}

bool GdalFeatureConnector::store(IlwisObject *obj, IlwisTypes type)
{
    return CoverageConnector::store(obj, type);
}
