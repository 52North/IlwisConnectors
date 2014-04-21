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
#include "size.h"

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
            quint64 grfId = addItem(handle, url, csyId, 0, itGEOREF,itCOORDSYSTEM);
            addItem(handle, url, csyId, grfId, itRASTER,itGEOREF | itNUMERICDOMAIN | itCONVENTIONALCOORDSYSTEM);
        } else{
            if ( count == 1) {//  default case, one layer per object
                OGRLayerH layerH = gdal()->getLayer(handle->handle(),0);
                int featureCount = gdal()->getFeatureCount(layerH, FALSE);
                addItem(handle, url, csyId, featureCount, itFEATURE , itCOORDSYSTEM | itTABLE);
                addItem(handle, url, 0, iUNDEF, itTABLE , 0);
                if (! mastercatalog()->id2Resource(csyId).isValid())
                    addItem(handle, QUrl(url), 0, iUNDEF, itCONVENTIONALCOORDSYSTEM , 0);
            }
            else { // multiple layers, the file itself will be marked as container; internal layers will be added using this file as container
                //TODO: atm the assumption is that all gdal containers are files. this is true in the majority of the cases but not in all. Without a proper testcase the non-file option will not(yet) be implemented
                addItem(handle, url, count, iUNDEF, itCATALOG , extTypes | itFILE);
                for(int i = 0; i < count; ++i){
                    OGRLayerH layerH = gdal()->getLayer(handle->handle(),i);
                    if ( layerH){
                        const char *cname = gdal()->getLayerName(layerH);
                        int featureCount = gdal()->getFeatureCount(layerH, FALSE);
                        if ( cname){
                            QString layerName(gdal()->getLayerName(layerH));
                            QString layerurl = url.toString() + "/" + layerName;
                            addItem(handle, QUrl(layerurl), csyId, featureCount, itFEATURE , itCOORDSYSTEM | itTABLE);
                            addItem(handle, QUrl(layerurl), 0, iUNDEF, itTABLE , 0);
                            if (! mastercatalog()->id2Resource(csyId).isValid())
                                addItem(handle, QUrl(layerurl), 0, iUNDEF, itCONVENTIONALCOORDSYSTEM , 0);
                        }

                    }

                }
            }
        }


        gdal()->closeFile(file.absoluteFilePath(), i64UNDEF);
    }
}

QString GDALItems::dimensions(GdalHandle* handle) const
{
    Size<> sz(gdal()->xsize(handle->handle()), gdal()->ysize(handle->handle()), gdal()->layerCount(handle->handle()));
    QString dim = QString("%1 x %2").arg(sz.xsize()).arg(sz.ysize());
    if ( sz.zsize() != 1){
        dim += " x " + QString::number(sz.zsize());
    }

    return dim;
}

quint64 GDALItems::addItem(GdalHandle* handle,const QUrl& url, quint64 csyid, quint64 grfId, IlwisTypes tp, IlwisTypes extTypes) {
    Resource gdalItem(url, tp);
    gdalItem.setExtendedType(extTypes);
    if ( !hasType(tp,itCATALOG))
        gdalItem.addProperty("coordinatesystem", csyid);
    if ( tp == itFEATURE){
        QString count = grfId == -1 ? "" : QString::number(grfId);
        gdalItem.dimensions(count);// misuse of grfid
    }
    else if ( tp == itRASTER){
        Resource resValue = mastercatalog()->name2Resource("code=value",itNUMERICDOMAIN);
        gdalItem.addProperty("domain", resValue.id());
        gdalItem.addProperty("georeference", grfId);
        QString dim = dimensions(handle);
        gdalItem.dimensions(dim);
    } else if (hasType(tp, itGEOREF)){
        gdalItem.dimensions(dimensions(handle));
    }else{
        if ( tp == itCATALOG){
            QString dim = QString::number(csyid); // misuse of csyid :)
            gdalItem.dimensions(dim);
        }
    }

    insert(gdalItem);

    return gdalItem.id();
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
        Envelope env = gdal()->envelope(handle,0);
        if ( env.isValid() && !env.isNull()){
            QString dim = QString("%1 x %2 x %3 x %4").arg(env.min_corner().x).arg(env.min_corner().y).arg(env.max_corner().x).arg(env.max_corner().y);
            resource.dimensions(dim);

        }
        insert(resource);
        return resource.id();
    }else
        return ret;
}




