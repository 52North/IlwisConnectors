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

    if ( type == wkbGeometryCollection )
        ret = itFEATURE;

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
//                    ITable attTable;
////                     createTable(fnBaseOutputName, dm, hFeatureDef, hLayer, tbl);
//                    if (!attTable.isValid())
//                        return false;
//                    fcoverage->attributeTable(attTable);

                    quint64 rec = 1;
                    OGRFeatureH hFeature;
                    gdal()->resetReading(hLayer);
                    bool ok = true;
                    while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL){
                        ok &= fillFeature(fcoverage, gdal()->getGeometryRef(hFeature), rec);
                        gdal()->destroyFeature( hFeature );
                    }
                }
            }
        }
    }
    return true;
}

bool GdalFeatureConnector::fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry, quint64& rec) const{
    if (geometry){
        switch (translateOGRType(gdal()->getGeometryType(geometry))){
            case itUNKNOWN: return ERROR2(ERR_INVALID_PROPERTY_FOR_2, "GeometryType of a Feature", _filename);
            case itPOINT:   return fillPoint(fcoverage, geometry, rec);    break;
            case itLINE:    return fillLine(fcoverage, geometry, rec);     break;
            case itPOLYGON: return fillPolygon(fcoverage, geometry, rec);  break;
            default:{ //possibly wbkGeometryCollection or other kind of mixture
                long subGeomCount = gdal()->getSubGeometryCount(geometry);
                if(subGeomCount){
                    bool ok;
                    for(int i = 0; i < subGeomCount; ++i) {
                        OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
                        if(hSubGeometry){
                            ok &= fillFeature(fcoverage, hSubGeometry, rec);
                        }else{
                            ok = false;
                        }
                    }
                    return ok;
                }
                break;
            }
        }
    }
    return false;
}
bool GdalFeatureConnector::fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry, quint64& rec) const{
    long subGeomCount = gdal()->getSubGeometryCount(geometry);
    if(subGeomCount){
        bool ok;
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                ok &= fillPoint(fcoverage, hSubGeometry, rec);
            }else{
                ok = false;
            }
        }
        return ok;
    }else{
        double x,y,z;
        gdal()->getPoints(geometry, 0,&x,&y,&z);
        Coordinate coord(x,y,z);
        Geometry point(coord);
        rec++;
        SPFeatureI feature = fcoverage->newFeature({point});
        if (feature != nullptr){
    //        tbl->cell(COVERAGEKEYCOLUMN, i, QVariant(itemId));
    //        tbl->cell(FEATUREIDCOLUMN, i, QVariant(feature->featureid()));
            return true;
        }else{
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, QString("Record: %1").arg(rec), _filename);
            return false;
        }
    }
}
bool GdalFeatureConnector::fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry, quint64& rec) const{
    long subGeomCount = gdal()->getSubGeometryCount(geometry);
    if(subGeomCount){
        bool ok;
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                ok &= fillLine(fcoverage, hSubGeometry, rec);
            }else{
                ok = false;
            }
        }
        return ok;
    }else{
        int count = gdal()->getPointCount(geometry);
         if ( count == 0)
             return true;

         Line2D<Coordinate2d > line;
         line.resize(count);
         for(int i = 0; i < count; ++i) {
             double x,y,z;
             gdal()->getPoints(geometry, i,&x,&y,&z);
             line[i] = Coordinate2d(x,y);
//             tbl->cell(FEATUREVALUECOLUMN, i, QVariant(z));
         }
        rec++;
        SPFeatureI feature = fcoverage->newFeature({line});
        if (feature != nullptr){
    //        tbl->cell(COVERAGEKEYCOLUMN, i, QVariant(itemId));
    //        tbl->cell(FEATUREIDCOLUMN, i, QVariant(feature->featureid()));
            return true;
        }else{
            return false;
        }
    }
}

bool GdalFeatureConnector::fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry, quint64& rec) const{
//    try {
//		if ( hGeometry) {
//			long count = funcs.ogrGetSubGeometryCount(hGeometry);
//			OGRwkbGeometryType tp = funcs.ogrGetGeometryType(hGeometry);
//			if ( tp == wkbPolygon || tp == wkbPolygon25D ){
//				fillPolygon(count, rec, hGeometry);
//				if ( isMulti == false) // for multis de raw remains the same (same record).
//					++rec;
//			}
//			else {//wbkMultiPolygon || wbkMultiPolygon25
//				for(int i = 0; i < count; ++i) {
//					OGRGeometryH hSubGeometry = funcs.ogrGetSubGeometry(hGeometry, i);
//					fillFeature(hSubGeometry, rec, count > 1);
//				}
//			}
//		}
//	} catch ( geos::util::IllegalArgumentException& ) {
//		// we ignore errors during import, polygons are skipped
//	}

    long subGeomCount = gdal()->getSubGeometryCount(geometry);
    if(subGeomCount){
        bool ok;
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                ok &= fillPolygon(fcoverage, hSubGeometry, rec);
            }else{
                ok = false;
            }
        }
        return ok;
    }else{
        int count = gdal()->getPointCount(geometry);
         if ( count == 0)
             return true;
//         Polygon pol;
//         readRing(stream, pol.outer());
//         double value;
//         quint32 numberOfHoles;
//         stream.readRawData((char *)&value, 8);
//         stream.readRawData((char *)&numberOfHoles, 4);
//         pol.inners().resize(numberOfHoles);
//         for(quint32 i=0; i< numberOfHoles;++i)
//             readRing(stream, pol.inners()[i]);
//         if ( isNumeric) {
//             tbl->cell(COVERAGEKEYCOLUMN, j, QVariant(j));
//             tbl->cell(FEATUREVALUECOLUMN, j, QVariant(value));
//             SPFeatureI feature = fcoverage->newFeature({pol});
//             tbl->cell(FEATUREIDCOLUMN, j, QVariant(feature->featureid()));
//         } else {
//             quint32 itemId = value;
//             tbl->cell(COVERAGEKEYCOLUMN, j, QVariant(itemId));
//             SPFeatureI feature = fcoverage->newFeature({pol});
//             tbl->cell(FEATUREIDCOLUMN, j, QVariant(feature->featureid()));
//         }

         Line2D<Coordinate2d > line;
         line.resize(count);
         for(int i = 0; i < count; ++i) {
             double x,y,z;
             gdal()->getPoints(geometry, i,&x,&y,&z);
             line[i] = Coordinate2d(x,y);
//             tbl->cell(FEATUREVALUECOLUMN, i, QVariant(z));
         }
        rec++;
        SPFeatureI feature = fcoverage->newFeature({line});
        if (feature != nullptr){
    //        tbl->cell(COVERAGEKEYCOLUMN, i, QVariant(itemId));
    //        tbl->cell(FEATUREIDCOLUMN, i, QVariant(feature->featureid()));
            return true;
        }else{
            return false;
        }
    }
}

bool GdalFeatureConnector::store(IlwisObject *obj, IlwisTypes type)
{
    return CoverageConnector::store(obj, type);
}
