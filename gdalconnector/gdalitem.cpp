#include <QString>
#include <QHash>
#include <QUrl>
#include <QLibrary>
#include <QStringList>
#include "kernel.h"
#include "identity.h"
#include "resource.h"
#include "gdalproxy.h"
#include "gdalitem.h"
#include "mastercatalog.h"

using namespace Ilwis;
using namespace Gdal;

GDALItems::GDALItems(const QString &path)
{
    GDALDatasetH dataSet = gdal()->open(path.toLocal8Bit(), GA_ReadOnly);

    int count  = gdal()->layerCount(dataSet);
    if ( count == 0)
        return;
    //TODO at the moment simplistic approach; all is corners georef and domain value
    // and a homogenous type if files. when we have example of more complex nature we wille xtend this
    QUrl url("file:///" + path);
    quint64 csyId = addCsy(dataSet, path);
    Resource resGrf(url, itGEOREF);
    resGrf.addProperty("coordinatesystem", csyId);
    resGrf.setExtendedType(itCONVENTIONALCOORDSYSTEM);
    insert(resGrf);
    if ( count == 1) {
        addItem(url, csyId, resGrf.id());
    } else {
//        for(int layer=0; layer < count; ++layer) {
//            QUrl url2(QString("file:///" + path + "#layerindex=%1").arg(layer));
//            addItem(url2,csyId, resGrf.id());
//        }
//        addItem(url, csyId, resGrf.id(), itRASTERCOVERAGELIST);
    }

    gdal()->close(dataSet);
}

void GDALItems::addItem(const QUrl& url, quint64 csyid, quint64 grfId, IlwisTypes tp) {
    Resource gdalItem(url, tp);
    Resource resValue = mastercatalog()->name2Resource("code=value",itNUMERICDOMAIN);
    gdalItem.addProperty("domain", resValue.id());
    gdalItem.addProperty("coordinatesystem", csyid);
    gdalItem.addProperty("georeference", grfId);
    gdalItem.setExtendedType(itGEOREF | itNUMERICDOMAIN | itCONVENTIONALCOORDSYSTEM);
    insert(gdalItem);
}

quint64 GDALItems::addCsy(GDALDatasetH dataSet, const QString &path) {
    OGRSpatialReferenceH srshandle = gdal()->srsHandle(dataSet, path);
    if ( srshandle == 0)
        return i64UNDEF;
    QString epsg(gdal()->getAttributeValue(srshandle,"AUTHORITY",0));
    if ( epsg != sUNDEF) {
        Resource res = mastercatalog()->name2Resource("code=epsg:" + epsg, itCONVENTIONALCOORDSYSTEM);
        if ( res.isValid())
            return res.id();
    }
    Resource res(QUrl("file:///" + path),itCONVENTIONALCOORDSYSTEM );
    insert(res);
    return res.id();
}




