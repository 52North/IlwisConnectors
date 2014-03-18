#include "kernel.h"
#include "ilwisdata.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "dataformat.h"
#include "gdalconnector.h"

using namespace Ilwis;
using namespace Gdal;

GdalConnector::GdalConnector(const Resource &resource, bool load, const PrepareOptions &options) : IlwisObjectConnector(resource,load, options), _internalPath(sUNDEF)
{
    _handle = NULL;
    if ( resource.url().hasFragment())
        _internalPath = resource.url().fragment();
    _filename = resource.url();
}

GdalConnector::~GdalConnector()
{
//    if ( _handle)
//        delete _handle;
    //TODO: delete causes a crash for unknown reasons; research later
}


IlwisTypes GdalConnector::ilwisType(const QString &name)
{
    QString filename = name;
    if (name.contains("?") == 0) {
        filename = name.split("?").front();
    }
    QFileInfo inf(filename);
    if ( inf.isDir())
        return itCATALOG;

    QString ext = inf.suffix();
    QString filter = "*." + ext;
    if ( gdal()->getExtensions(itRASTER).contains(filter,Qt::CaseInsensitive))
        return itRASTER;

    if ( gdal()->getExtensions(itFEATURE).contains(filter, Qt::CaseInsensitive))
        return itFEATURE;
    return itUNKNOWN; //TODO: add table formats here
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
    _handle = gdal()->openFile(fileinf, data->id(), GA_ReadOnly,false); // no messages here
    if (!_handle){ // could be a container based object
        QFileInfo inf = fileinf.absolutePath();
        _handle = gdal()->openFile(inf, data->id(), GA_ReadOnly);
    }
    if (!_handle){
        return ERROR2(ERR_COULD_NOT_OPEN_READING_2,_filename.toString(),QString(gdal()->getLastErrorMsg()));
    }
    data->setName(fileinf.fileName());

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

OGRFieldType GdalConnector::ilwisType2GdalFieldType(IlwisTypes tp) {
    if ( hasType(tp, itINTEGER))
        return OFTInteger;
    else if ( hasType(tp, (itFLOAT | itDOUBLE)))
        return OFTReal;
    else if ( tp == itTIME) // no hasType here as itDATETIME also contains itTIME
        return OFTTime;
    else if ( tp == itDATE)
        return OFTDate;
    else if ( tp == itDATETIME)
    if ( tp == itDATETIME)
        return OFTDateTime;
    return OFTString;
}

QString GdalConnector::constructOutputName(GDALDriverH hdriver) const
{
    const char *cext = gdal()->getMetaDataItem(hdriver,GDAL_DMD_EXTENSION,NULL);
    QFileInfo fileinfo = containerConnector(IlwisObject::cmOUTPUT)->toLocalFile(source());
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

OGRLayerH GdalConnector::getLayerHandle() const{
    int layer = 0;
    QFileInfo inf(source().toLocalFile());
    OGRLayerH hLayer = gdal()->getLayerByName(_handle->handle(), inf.fileName().toLatin1());
    if ( !hLayer) // so this was not a gdal container connector url, then the one layer per source case
         hLayer = gdal()->getLayer(_handle->handle(), layer);
    return hLayer;
}
