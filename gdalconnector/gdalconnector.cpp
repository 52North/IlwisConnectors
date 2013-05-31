#include "kernel.h"
#include "ilwisdata.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"

using namespace Ilwis;
using namespace Gdal;

GdalConnector::GdalConnector(const Resource &item, bool load) : IlwisObjectConnector(item,load), _internalPath(sUNDEF)
{
    if ( item.url().hasFragment())
        _internalPath = item.url().fragment();
    _filename = item.url().toLocalFile();
}


IlwisTypes Ilwis::Gdal::GdalConnector::ilwisType(const QString &name)
{
    QString filename = name;
    if (name.contains("?") == 0) {
        filename = name.split("?").front();
    }
    QFileInfo inf(filename);
    bool isCatalog =  inf.isDir();
    if ( isCatalog)
        return itCATALOG;
    QStringList extensions = gdal()->rasterNameFilter();
    if ( extensions.indexOf("." + inf.suffix())!= -1)
        return itGRIDCOVERAGE;
    return itUNKNOWN; //TODO vector and table formats here

}

bool GdalConnector::loadMetaData(IlwisObject *data)
{
    if ( _filename == "") {
        return ERROR1(ERR_MISSING_DATA_FILE_1,"Gdal reading");
    }
    _dataSet = gdal()->openFile(_filename, data->id());
    if (!_dataSet){
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_filename);
    }
    QFileInfo inf(_filename);
    data->setName(inf.fileName());
    return true;
}

bool GdalConnector::canUse(const Ilwis::Resource &item) {
    QStringList extensions = gdal()->rasterNameFilter();
    QFileInfo inf(item.url().toLocalFile());
    bool ok = extensions.indexOf("." + inf.suffix())!= -1;
    if ( ok)
        return true;
    GDALDatasetH handle = gdal()->open(item.url().toLocalFile().toLocal8Bit(), GA_ReadOnly);
    ok = handle != 0;
    gdal()->close(handle);

    return ok;
}
