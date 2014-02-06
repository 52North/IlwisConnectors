#include <QLibrary>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <functional>
#include "kernel.h"
#include "errorobject.h"
#include "ilwiscontext.h"
#include "dataformat.h"
#include "gdalproxy.h"


Ilwis::Gdal::GDALProxy* Ilwis::Gdal::GDALProxy::_proxy = 0;

Ilwis::Gdal::GDALProxy* Ilwis::Gdal::gdal() {
    if (Ilwis::Gdal::GDALProxy::_proxy == 0) {
        Ilwis::Gdal::GDALProxy::_proxy = new Ilwis::Gdal::GDALProxy("gdal.dll", "libproj-0.dll");
        Ilwis::Gdal::GDALProxy::_proxy->prepare();

    }
    if (!Ilwis::Gdal::GDALProxy::_proxy->isValid()) {
        throw InternalError(TR("Gdal library not properly initialized"));
    }
    return Ilwis::Gdal::GDALProxy::_proxy;
}

using namespace Ilwis;
using namespace Gdal;

GdalHandle::GdalHandle(void* h, GdalHandleType t, quint64 o): _handle(h),_type(t),_owner(o){}

GdalHandle::~GdalHandle()
{

}

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
    path = ilw.canonicalFilePath() + "/Extensions/gdalconnector/" + "expat.dll";
    _libexpat.setFileName(path);
    ok &= _libexpat.load();
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
    //GDAL Raster
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
    //Open Spatial Reference
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
    //OGR
    ogrOpen = add<IOGROpen>("OGROpen");
    releaseDataSource = add<IOGRReleaseDataSource>("OGRReleaseDataSource");
    ogrRegisterAll = add<IOGRRegisterAll>("OGRRegisterAll");
    ogrDriverCount =add<IOGRGetDriverCount>("OGRGetDriverCount");
    ogrGetDriver = add<IOGRGetDriver>("OGRGetDriver");
    getDriverByName = add<IOGRGetDriverByName>("OGRGetDriverByName");
    //OGR Driver
    getOGRDriverName = add<IOGRGetDriverName>("OGR_Dr_GetName");
    testDriverCapability = add<IOGRTestDriverCapability>("OGR_Dr_TestCapability");
    createDatasource = add<IOGR_Dr_CreateDataSource>("OGR_Dr_CreateDataSource");
    //OGR DataSource
    createOgrLayer = add<IOGR_DS_CreateLayer>("OGR_DS_CreateLayer");
    getLaterByName = add<IGetLayerByName>("OGR_DS_GetLayerByName");
    getLayerCount = add<IGetLayerCount>("OGR_DS_GetLayerCount");
    getLayer = add<IGetLayer>("OGR_DS_GetLayer");
    destroyDataSource = add<IOGR_DS_Destroy>("OGR_DS_Destroy");
    //OGR Layer
    getLayerName = add<IGetLayerName>("OGR_L_GetName");
    getLayerGeometry = add<IGetLayerGeometryType>("OGR_L_GetGeomType");
    resetReading = add<IResetReading>("OGR_L_ResetReading");
    getNextFeature = add<IGetNextFeature>("OGR_L_GetNextFeature");
    getLayerDef = add<IGetLayerDefn>("OGR_L_GetLayerDefn");
    getSpatialRef = add<IGetSpatialRef>("OGR_L_GetSpatialRef");
    getFeatureCount = add<IGetFeatureCount>("OGR_L_GetFeatureCount");
    getLayerExtent = add<IGetLayerExtent>("OGR_L_GetExtent");
    getSpatialFilter = add<IOGRGetSpatialFilter>("OGR_L_GetSpatialFilter");
    addAttribute = add<IOGR_L_CreateField>("OGR_L_CreateField");
    getLayerSchema = add<IOGR_L_GetLayerDefn>("OGR_L_GetLayerDefn");
    addFeature2Layer = add<IOGR_L_CreateFeature>("OGR_L_CreateFeature");
    //OGR FielDefinition
    getFieldCount = add<IGetFieldCount>("OGR_FD_GetFieldCount");
    getFieldDfn = add<IGetFieldDefn>("OGR_FD_GetFieldDefn");
    //OGR Field
    getFieldType = add<IGetFieldType>("OGR_Fld_GetType");
    getFieldName = add<IGetFieldName>("OGR_Fld_GetNameRef");
    createAttributeDefintion = add<IOGR_Fld_Create>("OGR_Fld_Create");
    destroyAttributeDefintion = add<IOGR_Fld_Destroy>("OGR_Fld_Destroy");
    //OGR Feature
    createFeature = add<IOGR_F_Create>("OGR_F_Create");
    getFieldIndex = add<IOGR_F_GetFieldIndex>("OGR_F_GetFieldIndex");
    setStringAttribute = add<IOGR_F_SetFieldString>("OGR_F_SetFieldString");
    setIntegerAttribute = add<IOGR_F_SetFieldInteger>("OGR_F_SetFieldInteger");
    setDoubleAttribute = add<IOGR_F_SetFieldDouble>("OGR_F_SetFieldDouble");
    setDateTimeAttribute = add<IOGR_F_SetFieldDateTime>("OGR_F_SetFieldDateTime");\
    getFieldAsString = add<IGetFieldAsString>("OGR_F_GetFieldAsString");
    getFieldAsDouble = add<IGetFieldAsDouble>("OGR_F_GetFieldAsDouble");
    getFieldAsInt = add<IGetFieldAsInteger>("OGR_F_GetFieldAsInteger");
    getFieldAsDateTime = add<IOGR_F_GetFieldAsDateTime>("OGR_F_GetFieldAsDateTime");\
    getGeometryRef = add<IGetGeometryRef>("OGR_F_GetGeometryRef");
    destroyFeature = add<IDestroyFeature>("OGR_F_Destroy");
    setGeometry = add<IOGR_F_SetGeometry>("OGR_F_SetGeometry");
    setGeometryDirectly = add<IOGR_F_SetGeometryDirectly>("OGR_F_SetGeometryDirectly");
    //OGR Geometry
    getGeometryType = add<IGetGeometryType>("OGR_G_GetGeometryType");
    getPointCount = add<IGetPointCount>("OGR_G_GetPointCount");
    getPoint = add<IGetPoints>("OGR_G_GetPoint");
    getSubGeometryCount = add<IGetSubGeometryCount>("OGR_G_GetGeometryCount");
    getSubGeometryRef = add<IGetSubGeometryRef>("OGR_G_GetGeometryRef");
    getEnvelope3D = add<IOGRGetEnvelope3D>("OGR_G_GetEnvelope3D");
    createGeometry = add<IOGR_G_CreateGeometry>("OGR_G_CreateGeometry");
    addPoint = add<IOGR_G_AddPoint>("OGR_G_AddPoint");
    addPoint2D = add<IOGR_G_AddPoint_2D>("OGR_G_AddPoint_2D");
    setPoint = add<IOGR_G_SetPoint>("OGR_G_SetPoint");
    setPoint2D = add<IOGR_G_SetPoint_2D>("OGR_G_SetPoint_2D");
    destroyGeometry = add<IOGR_G_DestroyGeometry>("OGR_G_DestroyGeometry");
    addGeometry = add<IOGR_G_AddGeometry>("OGR_G_AddGeometry");
    addGeometryDirectly = add<IOGR_G_AddGeometryDirectly>("OGR_G_AddGeometryDirectly");
    getCoordinateDimension = add<IOGR_G_GetCoordinateDimension>("OGR_G_GetCoordinateDimension");
    //CPL
    pushFinderLocation = add<ICPLPushFinderLocation>("CPLPushFinderLocation");
    getLastErrorMsg = add<ICPLGetLastErrorMsg>("CPLGetLastErrorMsg");
    //VSI
    vsiFileFromMem = add<IVSIFileFromMemBuffer>("VSIFileFromMemBuffer");
    vsiClose = add<IVSIFCloseL>("VSIFCloseL");
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

        DataFormat::setFormatInfo(path + "/ogr_formats.config","gdal");

    }

    return _isValid;
}

QString GDALProxy::translateOGRERR(char ogrErrCode){
    switch (ogrErrCode){
        case OGRERR_NONE: return "none"; break;
        case OGRERR_NOT_ENOUGH_DATA: return "not enough data"; break;
        case OGRERR_NOT_ENOUGH_MEMORY: return "not enough memory"; break;
        case OGRERR_UNSUPPORTED_GEOMETRY_TYPE: return "unsupported geometry type"; break;
        case OGRERR_UNSUPPORTED_OPERATION: return "unsupported operation"; break;
        case OGRERR_CORRUPT_DATA: return "corrupt data"; break;
        case OGRERR_FAILURE: return "failure"; break;
        case OGRERR_UNSUPPORTED_SRS: return "unsupported SRS"; break;
        case OGRERR_INVALID_HANDLE: return "invalid handle"; break;
        default: return "unknown OGR Error code!";
    }
}

bool GDALProxy::isValid() const
{
    return _isValid;
}

QStringList GDALProxy::getRasterExtensions() const
{
    return _rasterExtensions;
}

bool GDALProxy::supports(const Resource &resource) const{
    QFileInfo inf(resource.toLocalFile());
    QString ext = inf.suffix();
    QString filter = "*." + ext;
    if ( gdal()->getRasterExtensions().contains(filter,Qt::CaseInsensitive))
        return true;
    if ( DataFormat::supports(DataFormat::fpEXTENSION, itFEATURE,ext, "gdal"))
        return true;
    return false;
}

GdalHandle* GDALProxy::openFile(const QFileInfo& filename, quint64 asker, GDALAccess mode){
    void* handle = nullptr;
    auto name = filename.absoluteFilePath();
    if (_openedDatasets.contains(name)){
        return _openedDatasets[name];
    } else {
        handle = gdal()->ogrOpen(name.toLocal8Bit(), mode, NULL);
        if (handle){
            return _openedDatasets[name] = new GdalHandle(handle, GdalHandle::etOGRDataSourceH, asker);
        }else{
            handle = gdal()->open(name.toLocal8Bit(), mode);
            if (handle){
                return _openedDatasets[name] = new GdalHandle(handle, GdalHandle::etGDALDatasetH, asker);
            }else{
               ERROR1(ERR_COULD_NOT_OPEN_READING_1,name);
               return NULL;
            }
        }
    }
}

void GDALProxy::closeFile(const QString &filename, quint64 asker){
    QString name = filename;
    auto iter = _openedDatasets.find(name);
    if (iter != _openedDatasets.end() && iter.value()->_owner == asker) {
        GdalHandle* handle = _openedDatasets[name];
        if (handle->type() == GdalHandle::etGDALDatasetH){
            close(handle->handle());
        }else if(handle->etOGRDataSourceH){
            if (/*OGRErr err = */releaseDataSource((OGRDataSourceH)handle->handle()) != OGRERR_NONE){
                //TODO everything seems to work but, the error remains!
                //ERROR2("Couldn't release OGRDataSource (OGRERR %1) for %2", translateOGRERR(err) , name);
            }
        }else{
            ERROR2(ERR_INVALID_PROPERTY_FOR_2, "GDAL-OGR HandleType", name);
        }
        _openedDatasets.remove(name);
    }
}

OGRSpatialReferenceH GDALProxy::srsHandle(GdalHandle* handle, const QString& source) {
    if (handle != nullptr){
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
            char *pwkt;//TODO: only for debug?
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
