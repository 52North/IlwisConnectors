#include "kernel.h"
#include "ilwisdata.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"

using namespace Ilwis;
using namespace Gdal;

GdalConnector::GdalConnector(const Resource &resource, bool load) : IlwisObjectConnector(resource,load), _internalPath(sUNDEF)
{
    _handle = NULL;
    if ( resource.url().hasFragment())
        _internalPath = resource.url().fragment();
    _filename = resource.url();
}


IlwisTypes GdalConnector::ilwisType(const QString &name)
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

bool GdalConnector::loadMetaData(IlwisObject *data){
    if (data == nullptr)
        return false;

    if (!gdal()->isValid()) {
        return ERROR1(ERR_NO_INITIALIZED_1,"gdal library");
    }
    if ( !_filename.isValid()) {
        return ERROR1(ERR_MISSING_DATA_FILE_1,"Gdal reading");
    }

    QFileInfo fileinf = containerConnector()->toLocalFile(_filename);
    _handle = gdal()->openFile(fileinf, data->id(), GA_ReadOnly);
    if (!_handle){
        return ERROR2(ERR_COULD_NOT_OPEN_READING_2,_filename.toString(),QString(gdal()->getLastErrorMsg()));
    }
    data->setName(fileinf.fileName());

    return true;
}

bool GdalConnector::store(IlwisObject *, int )
{
    GDALDriverH hdriver = gdal()->getGDALDriverByName(_gdalShortName.toLocal8Bit());
    if ( !hdriver ) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, "data-source", _filename.toString());
    }
    const char* metaitem = gdal()->getMetaDataItem(hdriver, GDAL_DCAP_CREATE, NULL);
    if (QString(metaitem).toLower() != "yes") {
        return ERROR2(ERR_OPERATION_NOTSUPPORTED2, "write data-source", _filename.toString());
    }

    return true;
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
    QFileInfo fileinfo = containerConnector(IlwisObject::cmOUTPUT)->toLocalFile(_filename);
    QString filename = fileinfo.absoluteFilePath();
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
