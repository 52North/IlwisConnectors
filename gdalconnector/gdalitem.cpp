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

GDALItems::GDALItems(const QUrl &url, const UPContainerConnector &containerc)
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
        //TODO at the moment simplistic approach; all is corners georef and domain value
        // and a homogenous type if files. when we have example of more complex nature we wille xtend this+
        quint64 csyId = addCsy(handle, file, url);
        if ( handle->type() == GdalHandle::etGDALDatasetH) {
            Resource resGrf(url, itGEOREF);
            resGrf.addProperty("coordinatesystem", csyId);
            resGrf.setExtendedType(itCONVENTIONALCOORDSYSTEM);
            insert(resGrf);
            addItem(url, csyId, resGrf.id());
        } else
            addItem(url, csyId, iUNDEF, itFEATURE);


        gdal()->closeFile(file.absoluteFilePath(), i64UNDEF);
    }
}

void GDALItems::addItem(const QUrl& url, quint64 csyid, quint64 grfId, IlwisTypes tp) {
    Resource gdalItem(url, tp);
    gdalItem.addProperty("coordinatesystem", csyid);
    if ( tp == itRASTER){
        Resource resValue = mastercatalog()->name2Resource("code=value",itNUMERICDOMAIN);
        gdalItem.addProperty("domain", resValue.id());
        gdalItem.addProperty("georeference", grfId);
        gdalItem.setExtendedType(itGEOREF | itNUMERICDOMAIN | itCONVENTIONALCOORDSYSTEM);
    }else
       gdalItem.setExtendedType(itCONVENTIONALCOORDSYSTEM);

    insert(gdalItem);
}

quint64 GDALItems::addCsy(GdalHandle* handle, const QFileInfo &path, const QUrl& url) {
    OGRSpatialReferenceH srshandle = gdal()->srsHandle(handle, path.absoluteFilePath());
    if ( srshandle == 0)
        return i64UNDEF;
    QString epsg(gdal()->getAttributeValue(srshandle,"AUTHORITY",0));
    if ( epsg != sUNDEF) {
        Resource resource = mastercatalog()->name2Resource("code=epsg:" + epsg, itCONVENTIONALCOORDSYSTEM);
        if ( resource.isValid())
            return resource.id();
    }
    Resource resource(url,itCONVENTIONALCOORDSYSTEM );
    insert(resource);
    return resource.id();
}




