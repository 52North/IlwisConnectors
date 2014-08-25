#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "streamconnector.h"
#include "streamconnectorv1.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"


using namespace Ilwis;
using namespace Stream;

ConnectorInterface *StreamConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new StreamConnector(resource, load, options);

}

IlwisObject *StreamConnector::create() const
{
    return 0;
}


StreamConnector::StreamConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource,load,options)
{

}

StreamConnector::~StreamConnector()
{

}

bool StreamConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    if (!openSource(true))
        return false;
    QDataStream stream(_datasource.get());
    quint64 type;
    stream >> type;
    QString version;
    stream >> version;


    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (factory){
        _versionedConnector.reset( factory->create(version,type,stream));
    }

    if (!_versionedConnector)
        return false;

    return _versionedConnector->loadMetaData(data, options);
}

bool StreamConnector::openSource(bool reading){
    QUrl url = _resource.url(true);
    QString scheme = url.scheme();
    if ( scheme == "file"){
        QString filename = url.toLocalFile();
        QFile *file = new QFile(filename);

        if ( reading){
            if ( !file->exists()){
                delete file;
                return ERROR1(ERR_MISSING_DATA_FILE_1,file->fileName());
            }
        }
        if (file->open(reading  ? QIODevice::ReadOnly : QIODevice::ReadWrite)) {
            _datasource.reset(file);
            return true;
        }
    }
    return false;
}


bool StreamConnector::store(IlwisObject *obj, int options){
    if (!openSource(false))
        return false;
    QDataStream stream(_datasource.get());
    //stream << Version::IlwisVersion;

    const VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (factory){
        _versionedConnector.reset( factory->create(Version::IlwisVersion,_resource.ilwisType(),stream));
    }

    if (!_versionedConnector)
        return false;
    bool ok = _versionedConnector->store(obj, options);
    _datasource->close();
    _versionedConnector.reset(0);

    return ok;

}

QString StreamConnector::provider() const
{
    return "Stream";
}
