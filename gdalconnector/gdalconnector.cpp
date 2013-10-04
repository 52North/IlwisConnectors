#include "kernel.h"
#include "ilwisdata.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"

using namespace Ilwis;
using namespace Gdal;

GdalConnector::GdalConnector(const Resource &resource, bool load) : IlwisObjectConnector(resource,load), _internalPath(sUNDEF)
{
    if ( resource.url().hasFragment())
        _internalPath = resource.url().fragment();
    _filename = resource.url().toLocalFile();
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
    if ( gdal()->getRasterExtensions().indexOf("." + inf.suffix())!= -1)
        return itRASTER;
    if ( gdal()->getFeatureExtensions().indexOf("." + inf.suffix())!= -1)
        return itFEATURE;
    return itUNKNOWN; //TODO add table formats here

}

bool GdalConnector::loadMetaData(IlwisObject *data)
{
    if (!gdal()->isValid()) {
        return ERROR1(ERR_NO_INITIALIZED_1,"gdal library");
    }
    if ( _filename == "") {
        return ERROR1(ERR_MISSING_DATA_FILE_1,"Gdal reading");
    }
    return true;
}

bool GdalConnector::store(IlwisObject *, int )
{
    GDALDriverH hdriver = gdal()->getGDALDriverByName(_gdalShortName.toLocal8Bit());
    if ( !hdriver ) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, "data-source", _filename);
    }
    const char* metaitem = gdal()->getMetaDataItem(hdriver, GDAL_DCAP_CREATE, NULL);
    if (QString(metaitem).toLower() != "yes") {
        return ERROR2(ERR_OPERATION_NOTSUPPORTED2, "write data-source", _filename);
    }

    return true;
}

bool GdalConnector::canUse(const Ilwis::Resource &resource) {
    QStringList extensions = gdal()->getRasterExtensions();
    QFileInfo inf(resource.url().toLocalFile());
    bool ok = extensions.indexOf("." + inf.suffix())!= -1;
    if ( ok)
        return true;
    GDALDatasetH handle = gdal()->open(resource.url().toLocalFile().toLocal8Bit(), GA_ReadOnly);
    ok = handle != 0;
    gdal()->close(handle);

    return ok;
}

QString GdalConnector::provider() const
{
    return "gdal";
}

void GdalConnector::format(const QString &f)
{
    _gdalShortName = f;
}

QString GdalConnector::format() const
{
    return _gdalShortName;
}

GDALDataType GdalConnector::ilwisType2GdalType(IlwisTypes tp) {
    switch(tp) {
    case itINT8:
    case itUINT8:
        return GDT_Byte;
    case itINT16:
        return GDT_Int16;
    case itUINT16:
        return GDT_UInt16;
    case itINT32:
        return GDT_Int32;
    case itUINT32:
        return GDT_UInt32;
    case itFLOAT:
        return GDT_Float32;
    case itDOUBLE:
        return GDT_Float64;
    default:
        return GDT_Float64;
    }
}

QString GdalConnector::constructOutputName(GDALDriverH hdriver) const
{
    const char *cext = gdal()->getMetaDataItem(hdriver,GDAL_DMD_EXTENSION,NULL);
    QString filename = _filename;
    if ( cext != 0 ) {
        QString ext(cext);
        int index = filename.lastIndexOf(".");
        if ( index != -1) {
            QString pext = filename.right(filename.size() - index);
            if ( pext.toLower() != ext) {
                filename += "." + ext;
            }
        }else
           filename += "." + ext;
    }
    return filename;
}
