#include <QLibrary>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <functional>
#include "kernel.h"
#include "ilwiscontext.h"
#include "gdalproxy.h"


Ilwis::Gdal::GDALProxy* Ilwis::Gdal::GDALProxy::_proxy = 0;

Ilwis::Gdal::GDALProxy* Ilwis::Gdal::gdal() {
    if (Ilwis::Gdal::GDALProxy::_proxy == 0) {
        Ilwis::Gdal::GDALProxy::_proxy = new Ilwis::Gdal::GDALProxy("gdal.dll", "libproj-0.dll");
        Ilwis::Gdal::GDALProxy::_proxy->prepare();
    }
    return Ilwis::Gdal::GDALProxy::_proxy;
}

using namespace Ilwis;
using namespace Gdal;

GdalHandle::GdalHandle(void* h, GdalHandleType t, quint64 o): _handle(h),_type(t),_owner(o){}

GdalHandle::GdalHandleType GdalHandle::type(){
    return _type;
}
void* GdalHandle::handle(){
    return _handle;
}

GDALProxy::GDALProxy(const QString& gdalLibrary, const QString& proj4jLibrary) {
    QFileInfo ilw = context()->ilwisFolder();
    QString path = ilw.canonicalFilePath() + "/Extensions/gdalconnector/" + gdalLibrary;
    _libgdal.setFileName(path);
    path = ilw.canonicalFilePath() + "/Extensions/gdalconnector/" + proj4jLibrary;
    _libproj4.setFileName(path);
    bool ok = _libproj4.load();
    _isValid = _libgdal.load() && ok;
}

GDALProxy::~GDALProxy(){
//    foreach(GdalHandle* handle, _openedDatasets) {
//        if (handle->_type == GdalHandle::etGDALDatasetH){
//            close(handle->_handle);
//        }else{ //etOGRDataSourceH
//            if (OGRErr err = releaseDataSource(handle->_handle) != OGRERR_NONE){
//                ERROR2(ERR_INVALID_PROPERTY_FOR_2, QString("OGRDataSource (ERR %1)").arg(err), QString("%1").arg(handle->_owner));
//            }
//        }
//    }
}

bool GDALProxy::prepare() {
    close = add<IGDALClose>("GDALClose");
    registerAll = add<IGDALAllRegister>("GDALAllRegister");
    identifyDriver = add<IGDALIdentifyDriver>("GDALIdentifyDriver");
    open = add<IGDALOpen>("GDALOpen");
    xsize = add<IGDALGetSize>("GDALGetRasterXSize");
    ysize = add<IGDALGetSize>("GDALGetRasterYSize");
    layerCount = add<IGDALGetSize>("GDALGetRasterCount");
    getRasterBand = add<IGDALGetRasterBand>("GDALGetRasterBand");
    create = add<IGDALCreate>("GDALCreate");
    rasterDataType = add<IGDALGetRasterDataType>("GDALGetRasterDataType");
    getProjectionRef = add<IGDALGetProjectionRef>("GDALGetProjectionRef");
    setProjection = add<IGDALSetProjection>("GDALSetProjection");
    getGeotransform = add<IGDALGetGeoTransform>("GDALGetGeoTransform");
    setGeoTransform = add<IGDALSetGeoTransform>("GDALSetGeoTransform");
    rasterIO = add<IGDALRasterIO>("GDALRasterIO");
    getDataTypeSize = add<IGDALGetDataTypeSize>("GDALGetDataTypeSize");
    getAccess = add<IGDALGetAccess>("GDALGetAccess");
    getAttributeValue = add<IOSRGetAttrValue>("OSRGetAttrValue");
    getDriver = add<IGDALGetDriver>("GDALGetDriver");
    getGDALDriverByName = add<IGDALGetDriverByName>("GDALGetDriverByName");
    getDriverCount = add<IGDALGetDriverCount>("GDALGetDriverCount");
    getLongName = add<IGDALGetDriverName>("GDALGetDriverLongName");
    getShortName = add<IGDALGetDriverName>("GDALGetDriverShortName");
    getMetaDataItem = add<IGDALGetMetadataItem>("GDALGetMetadataItem");
    minValue = add<IGDALRasValue>("GDALGetRasterMinimum");
    maxValue = add<IGDALRasValue>("GDALGetRasterMaximum");
    colorInterpretation = add<IGDALGetRasterColorInterpretation>("GDALGetRasterColorInterpretation");

    importFromEpsg = add<IOSRImportFromEPSG>("OSRImportFromEPSG");
    exportToPrettyWkt = add<IOSRExportToPrettyWkt>("OSRExportToPrettyWkt");
    getProjectionParm = add<IOSRGetProjParm>("OSRGetProjParm");
    setWellKnownGeogCs = add<IOSRSetWellKnownGeogCS>("OSRSetWellKnownGeogCS");
    isProjected = add<IOSRIsProjectedFunc>("OSRIsProjected");
    authority = add<IOSRGetAuthorityCode>("OSRGetAuthorityCode");
    newSpatialRef = add<IOSRNewSpatialReference>("OSRNewSpatialReference");
    importFromWkt = add<IOSRImportFromWkt>("OSRImportFromWkt");
    exportToWkt = add<IExportToWkt>("OSRExportToWkt");
    importFromProj4 = add<IOSRImportFromProj4>("OSRImportFromProj4");

    ogrOpen = add<IOGROpen>("OGROpen");
    releaseDataSource = add<IOGRReleaseDataSource>("OGRReleaseDataSource");
    ogrRegisterAll = add<IOGRRegisterAll>("OGRRegisterAll");
    ogrDriverCount =add<IOGRGetDriverCount>("OGRGetDriverCount");
    ogrGetDriver = add<IOGRGetDriver>("OGRGetDriver");
    getDriverByName = add<IOGRGetDriverByName>("OGRGetDriverByName");
    getOGRDriverName = add<IOGRGetDriverName>("OGR_Dr_GetName");
    testDriverCapability = add<IOGRTestDriverCapability>("OGR_Dr_TestCapability");

    getLaterByName = add<IGetLayerByName>("OGR_DS_GetLayerByName");
    getLayerCount = add<IGetLayerCount>("OGR_DS_GetLayerCount");
    getLayer = add<IGetLayer>("OGR_DS_GetLayer");
    getLayerName = add<IGetLayerName>("OGR_L_GetName");
    getLayerGeometry = add<IGetLayerGeometryType>("OGR_L_GetGeomType");
    resetReading = add<IResetReading>("OGR_L_ResetReading");
    getNextFeature = add<IGetNextFeature>("OGR_L_GetNextFeature");
    getLayerDef = add<IGetLayerDefn>("OGR_L_GetLayerDefn");
    getFieldCount = add<IGetFieldCount>("OGR_FD_GetFieldCount");
    getFieldDfn = add<IGetFieldDefn>("OGR_FD_GetFieldDefn");
    getFieldType = add<IGetFieldType>("OGR_Fld_GetType");
    getFieldAsString = add<IGetFieldAsString>("OGR_F_GetFieldAsString");
    getFieldAsDouble = add<IGetFieldAsDouble>("OGR_F_GetFieldAsDouble");
    getFieldAsInt = add<IGetFieldAsInteger>("OGR_F_GetFieldAsInteger");
    getFieldAsDateTime = add<IGetFieldAsDateTime>("OGR_F_GetFieldAsDateTime");
    getGeometryRef = add<IGetGeometryRef>("OGR_F_GetGeometryRef");
    getGeometryType = add<IGetGeometryType>("OGR_G_GetGeometryType");
    destroyFeature = add<IDestroyFeature>("OGR_F_Destroy");
    getPointCount = add<IGetPointCount>("OGR_G_GetPointCount");
    getPoints = add<IGetPoints>("OGR_G_GetPoint");
    getSubGeometryCount = add<IGetSubGeometryCount>("OGR_G_GetGeometryCount");
    getSubGeometryRef = add<IGetSubGeometryRef>("OGR_G_GetGeometryRef");
    getSpatialRef = add<IGetSpatialRef>("OGR_L_GetSpatialRef");
    getFeatureCount = add<IGetFeatureCount>("OGR_L_GetFeatureCount");
    getLayerExtent = add<IGetLayerExtent>("OGR_L_GetExtent");
    getFieldName = add<IGetFieldName>("OGR_Fld_GetNameRef");
    getSpatialFilter = add<IOGRGetSpatialFilter>("OGR_L_GetSpatialFilter");
    getEnvelope3D = add<IOGRGetEnvelope3D>("OGR_G_GetEnvelope3D");
    destroyDataSource = add<IOGR_DS_Destroy>("OGR_DS_Destroy");

    pushFinderLocation = add<ICPLPushFinderLocation>("CPLPushFinderLocation");
    getLastErrorMsg = add<ICPLGetLastErrorMsg>("CPLGetLastErrorMsg");

    free = add<IFree>("VSIFree");

    if ( _isValid) {
        //raster extensions
        registerAll();
        int ndrivers = getDriverCount();
        QSet<QString> extensions;
        for(int index = 0; index < ndrivers; ++index) {
            GDALDriverH driverH = getDriver(index);
            if ( driverH) {
                const char *cext = getMetaDataItem(driverH,GDAL_DMD_EXTENSION,NULL);//raster extensions only
                if ( cext){
                    QString ext(cext);
                    if ( ext != "" && ext != "mpr/mpl")
                        extensions.insert(QString("*.%1").arg(cext));
                }
            }
        }
        _rasterExtensions = extensions.toList();
        QFileInfo ilw = context()->ilwisFolder();
        QString path = ilw.canonicalFilePath() + "/Extensions/gdalconnector/resources";
        pushFinderLocation(path.toLocal8Bit());
        //feature extensions
        ogrRegisterAll();
        QSettings fileExtensions(path.append("/ogr_extensions.ini"), QSettings::IniFormat);
        if (fileExtensions.childGroups().count() > 0){
            fileExtensions.beginGroup(fileExtensions.childGroups().at(0));
            for(QString key: fileExtensions.childKeys()){
                OGRSFDriverH hDriver = getDriverByName(fileExtensions.value(key).toString().toLocal8Bit());
                if (hDriver != nullptr && testDriverCapability(hDriver, ODrCCreateDataSource)){//ODrCDeleteDataSource is aso possible
                    _featureExtensions.append(QString("*.").append(key));
                }else{
                    ERROR2(ERR_NO_INITIALIZED_2,QString("File format (%1) support").arg(key),"OGR-Library (gdall.dll)");
                }
            }
        }else{
            _featureExtensions.append(QString("*.shp"));
        }
    }

    return _isValid;
}

bool GDALProxy::isValid() const
{
    return _isValid;
}

QStringList GDALProxy::getRasterExtensions() const
{
    return _rasterExtensions;
}

QStringList GDALProxy::getFeatureExtensions() const
{
    return _featureExtensions;
}

bool GDALProxy::supports(const Resource &resource) const{
    QFileInfo inf(resource.toLocalFile());
    QString ext = inf.suffix();
    QString filter = "*." + ext;
    if ( gdal()->getRasterExtensions().contains(filter,Qt::CaseInsensitive))
        return true;
    if ( gdal()->getFeatureExtensions().contains(filter,Qt::CaseInsensitive))
        return true;
    return false;
}

GdalHandle* GDALProxy::openFile(const QString& filename, quint64 asker, GDALAccess mode){
    void* handle = nullptr;
    auto name = filename.toLower();//TODO not on Linux! (MACRO?)
    if (_openedDatasets.contains(name)){
        return _openedDatasets[name];
    } else {
        handle = gdal()->ogrOpen(filename.toLocal8Bit(), mode, NULL);
        if (handle){
            return _openedDatasets[name] = new GdalHandle(handle, GdalHandle::etOGRDataSourceH, asker);
        }else{
            handle = gdal()->open(filename.toLocal8Bit(), mode);
            if (handle){
                return _openedDatasets[name] = new GdalHandle(handle, GdalHandle::etGDALDatasetH, asker);
            }else{
               ERROR1(ERR_COULD_NOT_OPEN_READING_1,filename);
               return NULL;
            }
        }
    }
}

void GDALProxy::closeFile(const QString &filename, quint64 asker){
    QString name = filename.toLower();
    auto iter = _openedDatasets.find(name);
    if (iter != _openedDatasets.end() && iter.value()->_owner == asker) {
        GdalHandle* handle = _openedDatasets[name];
        if (handle->type() == GdalHandle::etGDALDatasetH){
            close(handle->handle());
        }else if(handle->etOGRDataSourceH){
            if (OGRErr err = releaseDataSource(handle->handle()) != OGRERR_NONE){
                ERROR2(ERR_INVALID_PROPERTY_FOR_2, QString("OGRDataSource (ERR %1)").arg(err), name);
            }
        }else{
            ERROR2(ERR_INVALID_PROPERTY_FOR_2, "GDAL-OGR HandleType", name);
        }
        _openedDatasets.remove(name);
    }
}

OGRSpatialReferenceH GDALProxy::srsHandle(GdalHandle* handle, const QString& source) {
    if (handle == nullptr){
        OGRSpatialReferenceH srshandle = nullptr;
        if (handle->_type == GdalHandle::etGDALDatasetH){
            srshandle = newSpatialRef(NULL);
            const char *cwkt = getProjectionRef(handle->handle());
            if (!cwkt) {
                kernel()->issues()->log(TR("Invalid or empty WKT for %1 %2").arg("CoordinateSystem", source), IssueObject::itWarning);
                return NULL;
            }
            char wkt[5000];
            char *wkt2 = (char *)wkt;
            strcpy(wkt, cwkt);

            OGRErr err = importFromWkt(srshandle, &wkt2);
            char *pwkt;//TODO only for debug?
            exportToPrettyWkt(srshandle, &pwkt,0);
            free(pwkt);

            if ( err != OGRERR_NONE ){
                kernel()->issues()->log(TR(ERR_NO_OBJECT_TYPE_FOR_2).arg("CoordinateSystem", source), IssueObject::itWarning);
                return NULL;
            }
            return srshandle;
        }else if(handle->_type == GdalHandle::etOGRDataSourceH){
            OGRLayerH hLayer = getLayer(handle->handle(), 0);//take the first layer
            if (hLayer){
                srshandle = getSpatialRef(hLayer);
                if ( srshandle ){
                    return srshandle;
                }else{
                    kernel()->issues()->log(TR(ERR_NO_OBJECT_TYPE_FOR_2).arg("CoordinateSystem", source), IssueObject::itWarning);
                    return NULL;
                }
            }else{
                kernel()->issues()->log(TR(ERR_NO_OBJECT_TYPE_FOR_2).arg("CoordinateSystem", source), IssueObject::itWarning);
                return NULL;
            }
        }else{
            kernel()->issues()->log(TR(ERR_NO_OBJECT_TYPE_FOR_2).arg("CoordinateSystem", QString("%1 : nullptr").arg(source)), IssueObject::itWarning);
            return NULL;
        }
    }else{
        return NULL;
    }
}
