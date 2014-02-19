#include <QString>
#include <QHash>
#include <QUrl>
#include <QLibrary>
#include <QStringList>
#include "kernel.h"
#include "identity.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "resource.h"
#include "gdalproxy.h"
#include "gdalitem.h"
#include "mastercatalog.h"

using namespace Ilwis;
using namespace Gdal;

GDALItems::GDALItems(const QUrl &url, const UPContainerConnector &containerc, IlwisTypes extTypes)
{
    if ( !containerc || !containerc->isValid())
        return ;
    QFileInfo file = containerc->toLocalFile(url);
    GdalHandle* handle = gdal()->openFile(file.absoluteFilePath(), i64UNDEF , GA_ReadOnly);
    if (handle){
        int count = 0;
        if (handle->type() == GdalHandle::etGDALDatasetH){
            count = gdal()->layerCount(handle->handle());
        }else if(handle->type() == GdalHandle::etOGRDataSourceH){
            count = gdal()->getLayerCount(handle->handle());
        }
        if ( count == 0)
            return;
        //TODO: at the moment simplistic approach; all is corners georef and domain value
        // and a homogenous type if files. when we have example of more complex nature we wille xtend this+
        quint64 csyId = addCsy(handle, file, url);
        if ( handle->type() == GdalHandle::etGDALDatasetH) {
            Resource resGrf(url, itGEOREF);
            resGrf.addProperty("coordinatesystem", csyId);
            resGrf.setExtendedType(itCONVENTIONALCOORDSYSTEM);
            insert(resGrf);
            addItem(url, csyId, resGrf.id());
        } else
            addItem(url, csyId, iUNDEF, itFEATURE, extTypes);


        gdal()->closeFile(file.absoluteFilePath(), i64UNDEF);
    }
}

void GDALItems::addItem(const QUrl& url, quint64 csyid, quint64 grfId, IlwisTypes tp, IlwisTypes extTypes) {
    Resource gdalItem(url, tp);
    gdalItem.addProperty("coordinatesystem", csyid);
    if ( tp == itRASTER){
        Resource resValue = mastercatalog()->name2Resource("code=value",itNUMERICDOMAIN);
        gdalItem.addProperty("domain", resValue.id());
        gdalItem.addProperty("georeference", grfId);
        gdalItem.setExtendedType(itGEOREF | itNUMERICDOMAIN | itCONVENTIONALCOORDSYSTEM);
    }else
       gdalItem.setExtendedType(itCONVENTIONALCOORDSYSTEM | extTypes);

    insert(gdalItem);
}

quint64 GDALItems::addCsy(GdalHandle* handle, const QFileInfo &path, const QUrl& url) {
    quint64 ret = i64UNDEF;
    OGRSpatialReferenceH srshandle = gdal()->srsHandle(handle, path.absoluteFilePath());
    if (srshandle != 0){
        const char * projcs_epsg = gdal()->getAuthorityCode(srshandle, "PROJCS");
        const char * geocs_epsg = gdal()->getAuthorityCode(srshandle, "GEOGCS");
        if (QString(gdal()->getAuthorityName(srshandle, "PROJCS")).compare("EPSG", Qt::CaseInsensitive) == 0 && projcs_epsg) {
            Resource resource = mastercatalog()->name2Resource(QString("code=epsg:%1").arg(projcs_epsg), itCONVENTIONALCOORDSYSTEM);
            if ( resource.isValid())
                ret = resource.id();
        }else if (QString(gdal()->getAuthorityName(srshandle, "GEOGCS")).compare("EPSG", Qt::CaseInsensitive) == 0 && geocs_epsg){
            Resource resource = mastercatalog()->name2Resource(QString("code=epsg:%1").arg(geocs_epsg), itCONVENTIONALCOORDSYSTEM);
            if ( resource.isValid())
                ret = resource.id();
        }
    }
    gdal()->releaseSrsHandle(handle, srshandle, path.absoluteFilePath());

    if(ret == i64UNDEF){
        Resource resource(url,itCONVENTIONALCOORDSYSTEM );
        insert(resource);
        return resource.id();
    }else
        return ret;
}




