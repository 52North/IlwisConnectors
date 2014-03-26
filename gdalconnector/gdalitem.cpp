#include <QString>
#include <QHash>
#include <QUrl>
#include <QLibrary>
#include <QStringList>
#include "kernel.h"
#include "identity.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "resource.h"
#include "gdalproxy.h"
#include "gdalitem.h"
#include "mastercatalog.h"

using namespace Ilwis;
using namespace Gdal;

GDALItems::GDALItems(const QUrl &url, const QFileInfo &localFile, IlwisTypes extTypes)
{
    if ( localFile.isRoot())
        return ;
    QFileInfo file = localFile;
    GdalHandle* handle = gdal()->openFile(file.absoluteFilePath(), i64UNDEF , GA_ReadOnly, false);
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
        quint64 csyId = addCsy(handle, file, url, false);
        if ( handle->type() == GdalHandle::etGDALDatasetH) {
            Resource resGrf(url, itGEOREF);
            resGrf.addProperty("coordinatesystem", csyId);
            resGrf.setExtendedType(itCONVENTIONALCOORDSYSTEM);
            insert(resGrf);
            addItem(url, csyId, resGrf.id());
        } else{
            if ( count == 1) {//  default case, one layer per object
                addItem(url, csyId, iUNDEF, itFEATURE , itCOORDSYSTEM | itTABLE);
                addItem(url, 0, iUNDEF, itTABLE , 0);
                if (! mastercatalog()->id2Resource(csyId).isValid())
                    addItem(QUrl(url), 0, iUNDEF, itCONVENTIONALCOORDSYSTEM , 0);
            }
            else { // multiple layers, the file itself will be marked as container; internal layers will be added using this file as container
                //TODO: atm the assumption is that all gdal containers are files. this is true in the majority of the cases but not in all. Without a proper testcase the non-file option will not(yet) be implemented
                addItem(url, iUNDEF, iUNDEF, itCATALOG , extTypes | itFILE);
                for(int i = 0; i < count; ++i){
                    OGRLayerH layerH = gdal()->getLayer(handle->handle(),i);
                    if ( layerH){
                        const char *cname = gdal()->getLayerName(layerH);
                        if ( cname){
                            QString layerName(gdal()->getLayerName(layerH));
                            QString layerurl = url.toString() + "/" + layerName;
                            addItem(QUrl(layerurl), csyId, iUNDEF, itFEATURE , itCOORDSYSTEM | itTABLE);
                            addItem(QUrl(layerurl), 0, iUNDEF, itTABLE , 0);
                            if (! mastercatalog()->id2Resource(csyId).isValid())
                                addItem(QUrl(layerurl), 0, iUNDEF, itCONVENTIONALCOORDSYSTEM , 0);
                        }

                    }

                }
            }
        }


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
       gdalItem.setExtendedType(extTypes);

    insert(gdalItem);
}

quint64 GDALItems::addCsy(GdalHandle* handle, const QFileInfo &path, const QUrl& url, bool message) {
    quint64 ret = i64UNDEF;
    OGRSpatialReferenceH srshandle = gdal()->srsHandle(handle, path.absoluteFilePath(), message);
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
    if ( srshandle)
        gdal()->releaseSrsHandle(handle, srshandle, path.absoluteFilePath());

    if(ret == i64UNDEF){
        Resource resource(url,itCONVENTIONALCOORDSYSTEM );
        insert(resource);
        return resource.id();
    }else
        return ret;
}




