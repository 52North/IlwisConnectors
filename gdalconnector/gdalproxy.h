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
typedef CPLErr (*IGDALSetProjection)(GDALDatasetH,const char *);
typedef CPLErr (*IGDALGetGeoTransform )(GDALDatasetH, double *) ;
typedef CPLErr (*IGDALSetGeoTransform)(GDALDatasetH, double * );
typedef CPLErr (*IGDALRasterIO )(GDALRasterBandH , GDALRWFlag , int , int , int , int , void *, int , int , GDALDataType , int , int ) ;
typedef int (*IGDALGetDataTypeSize )(GDALDataType) ;
typedef int (*IGDALGetAccess )(GDALDatasetH) ;
typedef GDALDriverH (*IGDALGetDriver )(int) ;
typedef int (*IGDALGetDriverCount )() ;
typedef GDALDriverH (*IGDALGetDriverByName)(const char * ) ;
typedef const char* (*IGDALGetDriverName )(GDALDriverH) ;
typedef const char* (*IGDALGetMetadataItem )(GDALMajorObjectH , const char *, const char *) ;
typedef double (*IGDALRasValue)(GDALRasterBandH, int * );
typedef GDALColorInterp (*IGDALGetRasterColorInterpretation)(GDALRasterBandH);

typedef OGRErr (*IOSRImportFromWkt )(OGRSpatialReferenceH, char **) ;
typedef OGRErr (*IOSRSetWellKnownGeogCS )(OGRSpatialReferenceH, char *) ;
typedef int (*IOSRIsProjectedFunc )(OGRSpatialReferenceH) ;
typedef const char* (*IOSRGetAttrValue )(OGRSpatialReferenceH, const char *, int) ;
typedef OGRErr (*IOSRImportFromEPSG )(OGRSpatialReferenceH, int) ;
typedef OGRErr (*IOSRExportToPrettyWkt )(OGRSpatialReferenceH ,char **,int) ;
typedef double (*IOSRGetProjParm )(OGRSpatialReferenceH,const char *,double ,OGRErr *) ;
typedef const char* (*IOSRGetAuthorityCode)( OGRSpatialReferenceH, const char *);

typedef OGRDataSourceH (*IOGROpen )(const char *, int, OGRSFDriverH *) ;
typedef void (*IOGRRegisterAll )() ;
typedef int (*IOGRGetDriverCount )() ;
typedef OGRSFDriverH (*IOGRGetDriver )(int) ;
typedef const char * (*IOGRGetDriverName )(OGRSFDriverH) ;
typedef OGRSFDriverH  (*IOGRGetDriverByName )(const char *) ;
typedef int (*IOGRTestDriverCapability)(OGRSFDriverH ,const char *);
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
typedef int (*IGetFieldAsDateTime) (OGRFeatureH, int, int *, int *, int *, int *, int *, int *, int *) ;
typedef OGRGeometryH (*IGetGeometryRef )(OGRFeatureH ) ;
typedef OGRwkbGeometryType (*IGetGeometryType )(OGRGeometryH) ;
typedef void (*IDestroyFeature )(OGRFeatureH ) ;
typedef int (*IGetPointCount )(OGRGeometryH) ;
typedef void (*IGetPoints )(OGRGeometryH,int,double *,double *,double *) ;
typedef int (*IGetSubGeometryCount )(OGRGeometryH)  ;
typedef OGRGeometryH (*IGetSubGeometryRef )(OGRGeometryH,int) ;
typedef OGRSpatialReferenceH (*IGetSpatialRef )(OGRLayerH hLayer) ;
typedef OGRErr (*IExportToWkt )(OGRSpatialReferenceH,char **) ;
typedef OGRErr (*IOSRImportFromProj4)(OGRSpatialReferenceH, const char *);
typedef int (*IGetFeatureCount )(OGRLayerH,int) ;
typedef OGRErr 	(*IGetLayerExtent )(OGRLayerH, OGREnvelope *, int) ;
typedef const char * (*IGetFieldName )(OGRFieldDefnH) ;
typedef void (*ICPLPushFinderLocation )( const char * ) ;
typedef OGRErr (*IOGRReleaseDataSource) (OGRDataSourceH);
typedef OGRGeometryH (*IOGRGetSpatialFilter)(OGRLayerH);
typedef void (*IOGRGetEnvelope3D) (OGRGeometryH, OGREnvelope*);
typedef const char* (*ICPLGetLastErrorMsg)();
typedef void (*IFree)( void * );
typedef int (*IOGRGetGeomFieldCoun) (OGRFeatureH);
typedef void (*IOGR_DS_Destroy) (OGRDataSourceH);


class GdalHandle {
    friend class GDALProxy;
    public:
        enum GdalHandleType{
            etGDALDatasetH,
            etOGRDataSourceH
        };

        GdalHandle(void* h, GdalHandleType t, quint64 o=i64UNDEF);
        GdalHandleType type();
        void* handle();
    private:
        void* _handle;
        GdalHandleType _type;
        quint64 _owner;
};

class GDALProxy {
    friend GDALProxy* gdal();

public:


public:
    GDALProxy(const QString& gdalLibrary, const QString& proj4jLibrary);
    ~GDALProxy();

    bool isValid() const;
    QStringList getRasterExtensions() const;
    QStringList getFeatureExtensions() const;
    bool supports(const Resource& resource) const;

    template<class T> T  add(const QString& name) {
        if (!_isValid)
            return 0;

        T fp =  (T )(_libgdal.resolve(name.toLocal8Bit()));
        if ( fp == 0) {
            kernel()->issues()->log(TR("GDAL-proxy not properly initialized; can't find %1").arg(name));
            _isValid = false;
        }
        return fp;
    }

    GdalHandle* openFile(const QString& filename, quint64 asker, GDALAccess mode=GA_ReadOnly);
    void closeFile(const QString& filename, quint64 asker);
    OGRSpatialReferenceH srsHandle(GdalHandle* handle, const QString& source);

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
    IGDALSetProjection setProjection;
    IGDALGetGeoTransform getGeotransform;
    IGDALSetGeoTransform setGeoTransform;
    IGDALRasterIO rasterIO;
    IGDALGetDataTypeSize getDataTypeSize;
    IGDALGetAccess getAccess;
    IGDALGetDriver getDriver;
    IGDALGetDriverByName getGDALDriverByName;
    IGDALGetDriverCount getDriverCount;
    IGDALGetDriverName getLongName;
    IGDALGetDriverName getShortName;
    IGDALGetMetadataItem getMetaDataItem;
    IGDALGetRasterColorInterpretation colorInterpretation;

    IOSRNewSpatialReference newSpatialRef;
    IOSRImportFromWkt importFromWkt;
    IOSRSetWellKnownGeogCS setWellKnownGeogCs;
    IOSRIsProjectedFunc isProjected;
    IOSRGetAttrValue getAttributeValue;
    IOSRImportFromEPSG importFromEpsg;
    IOSRExportToPrettyWkt exportToPrettyWkt;
    IOSRGetProjParm getProjectionParm;
    IOSRGetAuthorityCode authority;
    IOSRImportFromProj4 importFromProj4;

    IOGROpen ogrOpen;
    IOGRRegisterAll ogrRegisterAll;
    IOGRGetDriverCount ogrDriverCount;
    IOGRGetDriver ogrGetDriver;
    IOGRGetDriverName getOGRDriverName;
    IOGRGetDriverByName getDriverByName;
    IOGRTestDriverCapability testDriverCapability;
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
    IGetFieldAsDateTime getFieldAsDateTime;
    IGetGeometryRef getGeometryRef;
    IGetGeometryType getGeometryType;
    IDestroyFeature destroyFeature;
    IGetPointCount getPointCount;
    IGetPoints getPoints;
    IGetSubGeometryCount getSubGeometryCount;
    IGetSubGeometryRef getSubGeometryRef;
    IGetSpatialRef getSpatialRef;
    IExportToWkt exportToWkt;
    IGetFeatureCount getFeatureCount;
    IGetLayerExtent getLayerExtent;
    IGetFieldName getFieldName;
    ICPLPushFinderLocation pushFinderLocation;
    ICPLGetLastErrorMsg getLastErrorMsg;
    IOGRReleaseDataSource releaseDataSource;
    IOGRGetSpatialFilter getSpatialFilter;
    IOGRGetEnvelope3D getEnvelope3D;
    IOGR_DS_Destroy destroyDataSource;
    IFree free;

private:
    bool prepare();

    QLibrary _libgdal, _libproj4;
    bool _isValid;
    static GDALProxy *_proxy;
    QStringList _featureExtensions;
    QStringList _rasterExtensions;
    QHash<QString, GdalHandle*> _openedDatasets;
};

GDALProxy *gdal();
}

}

#endif // GDALPROXY_H
