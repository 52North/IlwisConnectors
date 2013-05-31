#ifndef GDALPROXY_H
#define GDALPROXY_H

#include <QHash>
#include "ogr_api.h"
#include "gdal.h"

namespace Ilwis{
namespace Gdal{

typedef void *(*IGDALClose)(GDALDatasetH) ;
typedef void *(*IGDALAllRegister )() ;
typedef GDALDriverH (*IGDALIdentifyDriver )(const char *, char **) ;
typedef GDALDatasetH (*IGDALOpen )(const char *, GDALAccess) ;
typedef int (*IGDALGetSize )(GDALDatasetH) ;
typedef GDALRasterBandH (*IGDALGetRasterBand )(GDALDatasetH, int) ;
typedef GDALDatasetH (*IGDALCreate )(GDALDriverH hDriver, const char *, int, int, int, GDALDataType, char **) ;
typedef GDALDataType (*IGDALGetRasterDataType )(GDALRasterBandH) ;
typedef char * (*IGDALGetProjectionRef )(GDALDatasetH) ;
typedef OGRSpatialReferenceH (*IOSRNewSpatialReference )(const char *) ;
typedef OGRErr (*IOSRImportFromWkt )(OGRSpatialReferenceH, char **) ;
typedef OGRErr (*IOSRSetWellKnownGeogCS )(OGRSpatialReferenceH, char *) ;
typedef int (*IOSRIsProjectedFunc )(OGRSpatialReferenceH) ;
typedef CPLErr (*IGDALGetGeoTransform )(GDALDatasetH, double *) ;
typedef CPLErr (*IGDALRasterIO )(GDALRasterBandH , GDALRWFlag , int , int , int , int , void *, int , int , GDALDataType , int , int ) ;
typedef int (*IGDALGetDataTypeSize )(GDALDataType) ;
typedef int (*IGDALGetAccess )(GDALDatasetH) ;
typedef const char* (*IOSRGetAttrValue )(OGRSpatialReferenceH, const char *, int) ;
typedef GDALDriverH (*IGDALGetDriver )(int) ;
typedef int (*IGDALGetDriverCount )() ;
typedef const char* (*IGDALGetDriverName )(GDALDriverH) ;
typedef const char* (*IGDALGetMetadataItem )(GDALMajorObjectH , const char *, const char *) ;
typedef OGRErr (*IOSRImportFromEPSG )(OGRSpatialReferenceH, int) ;
typedef OGRErr (*IOSRExportToPrettyWkt )(OGRSpatialReferenceH ,char **,int) ;
typedef double (*IOSRGetProjParm )(OGRSpatialReferenceH,const char *,double ,OGRErr *) ;
typedef double (*IGDALRasValue)(GDALRasterBandH, int * );
typedef GDALColorInterp (*IGDALGetRasterColorInterpretation)(GDALRasterBandH);
typedef const char* (*IOSRGetAuthorityCode)( OGRSpatialReferenceH, const char *);

typedef OGRDataSourceH (*IOGROpen )(const char *, int, OGRSFDriverH *) ;
typedef void (*IOGRRegisterAll )() ;
typedef int (*IOGRGetDriverCount )() ;
typedef OGRSFDriverH (*IOGRGetDriver )(int) ;
typedef const char * (*IOGRGetDriverName )(OGRSFDriverH) ;
typedef OGRSFDriverH  (*IOGRGetDriverByName )(const char *) ;
typedef OGRLayerH (*IGetLayerByName )(OGRDataSourceH, const char *) ;
typedef int (*IGetLayerCount )(OGRDataSourceH) ;
typedef OGRLayerH (*IGetLayer )(OGRDataSourceH, int) ;
typedef const char * (*IGetLayerName )(OGRLayerH) ;
typedef OGRwkbGeometryType (*IGetLayerGeometryType )(OGRLayerH ) ;
typedef void (*IResetReading )(OGRLayerH) ;
typedef OGRFeatureH (*IGetNextFeature )(OGRLayerH ) ;
typedef OGRFeatureDefnH (*IGetLayerDefn )(OGRLayerH ) ;
typedef int (*IGetFieldCount )( OGRFeatureDefnH )  ;
typedef OGRFieldDefnH (*IGetFieldDefn )(OGRFeatureDefnH,int ) ;
typedef OGRFieldType (*IGetFieldType )(OGRFieldDefnH 	hDefn ) ;
typedef int (*IGetFieldAsInteger )(OGRFeatureH,int) ;
typedef double (*IGetFieldAsDouble )(OGRFeatureH,int) ;
typedef const char* (*IGetFieldAsString )(OGRFeatureH,int) ;
typedef OGRGeometryH (*IGetGeometryRef )(OGRFeatureH ) ;
typedef OGRwkbGeometryType (*IGetGeometryType )(OGRGeometryH) ;
typedef void (*IDestroyFeature )(OGRFeatureH ) ;
typedef int (*IGetPointCount )(OGRGeometryH) ;
typedef void (*IGetPoints )(OGRGeometryH,int,double *,double *,double *) ;
typedef int (*IGetSubGeometryCount )(OGRGeometryH)  ;
typedef OGRGeometryH (*IGetSubGeometryRef )(OGRGeometryH,int) ;
typedef OGRSpatialReferenceH (*IGetSpatialRef )(OGRLayerH hLayer) ;
typedef OGRErr (*IExportToWkt )(OGRSpatialReferenceH,char **) ;
typedef int (*IGetFeatureCount )(OGRLayerH,int) ;
typedef OGRErr 	(*IGetLayerExtent )(OGRLayerH, OGREnvelope *, int) ;
typedef const char * (*IGetFieldName )(OGRFieldDefnH) ;
typedef void (*ICPLPushFinderLocation )( const char * ) ;


class GDALProxy {
    friend GDALProxy* gdal();

    struct GdalHandle {
        GdalHandle(GDALDatasetH h=0, quint64 o=0) : _handle(h),_owner(o) {}
        GDALDatasetH _handle;
        quint64 _owner;
    };

public:
    GDALProxy(const QString& library);
    ~GDALProxy();

    bool isValid() const;
    QStringList rasterNameFilter() const;

    template<class T> T  add(const QString& name) {
        if (!_isValid)
            return 0;

        T fp =  (T )(_lib.resolve(name.toLocal8Bit()));
        if ( fp == 0) {
            kernel()->issues()->log(TR("GDAL-proxy not properly initialized; can't find %1").arg(name));
            _isValid = false;
        }
        return fp;
    }

    GDALDatasetH openFile(const QString& filename, quint64 asker, GDALAccess mode=GA_ReadOnly);
    void closeFile(const QString& filename, quint64 asker);
    GDALDatasetH operator [] (const QString& filename);
    OGRSpatialReferenceH srsHandle(GDALDatasetH dataSet, const QString &source);

    IGDALClose close;
    IGDALOpen open;
    IGDALAllRegister registerAll;
    IGDALIdentifyDriver identifyDriver;
    IGDALGetSize xsize;
    IGDALGetSize ysize;
    IGDALGetSize layerCount;
    IGDALRasValue minValue;
    IGDALRasValue maxValue;
    IGDALGetRasterBand getRasterBand;
    IGDALCreate create;
    IGDALGetRasterDataType rasterDataType;
    IGDALGetProjectionRef getProjectionRef;
    IOSRNewSpatialReference newSpatialRef;
    IOSRImportFromWkt importFromWkt;
    IOSRSetWellKnownGeogCS setWellKnownGeogCs;
    IOSRIsProjectedFunc isProjected;
    IGDALGetGeoTransform getGeotransform;
    IGDALRasterIO rasterIO;
    IGDALGetDataTypeSize getDataTypeSize;
    IGDALGetAccess getAccess;
    IOSRGetAttrValue getAttributeValue;
    IGDALGetDriver getDriver;
    IGDALGetDriverCount getDriverCount;
    IGDALGetDriverName getLongName;
    IGDALGetDriverName getShortName;
    IGDALGetMetadataItem getMetaDataItem;
    IOSRImportFromEPSG importFromEpsg;
    IOSRExportToPrettyWkt exportToPrettyWkt;
    IOSRGetProjParm getProjectionParm;
    IGDALGetRasterColorInterpretation colorInterpretation;
    IOSRGetAuthorityCode authority;

    IOGROpen ogrOpen;
    IOGRRegisterAll ogrRegisterAll;
    IOGRGetDriverCount ogrDriverCount;
    IOGRGetDriver ogrGetDriver;
    IOGRGetDriverName getDriverName;
    IOGRGetDriverByName getDriverByName;
    IGetLayerByName getLaterByName;
    IGetLayerCount getLayerCount;
    IGetLayer getLayer;
    IGetLayerName getLayerName;
    IGetLayerGeometryType getLayerGeometry;
    IResetReading resetReading;
    IGetNextFeature getNextFeature;
    IGetLayerDefn getLayerDef;
    IGetFieldCount getFieldCount;
    IGetFieldDefn getFieldDfn;
    IGetFieldType getFieldType;
    IGetFieldAsInteger getFieldAsInt;
    IGetFieldAsDouble getFieldAsDouble;
    IGetFieldAsString getFieldAsString;
    IGetGeometryRef getGeometryRef;
    IGetGeometryType getGeometryType;
    IDestroyFeature destroyFeature;
    IGetPointCount getPointCount;
    IGetPoints getPoints;
    IGetSubGeometryCount getSubGeometry;
    IGetSubGeometryRef getSubGeometryRef;
    IGetSpatialRef getSpatialRef;
    IExportToWkt exportToWkt;
    IGetFeatureCount featureCount;
    IGetLayerExtent getLayerExtent;
    IGetFieldName getFieldName;
    ICPLPushFinderLocation pushFinderLocation;


private:
    bool prepare();

    QLibrary _lib;
    bool _isValid;
    static GDALProxy *_proxy;
    QStringList _rasterExtensions;
    QHash<QString, GdalHandle> _openedDatasets;
};

GDALProxy *gdal();
}

}

#endif // GDALPROXY_H
