#include <QBuffer>
#include "raster.h"
#include "version.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "rawconverter.h"
#include "downloadmanager.h"
#include "versioneddatastreamfactory.h"


using namespace Ilwis;
using namespace Stream;

ConnectorInterface *StreamConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new StreamConnector(resource, load, options);

}

IlwisObject *StreamConnector::create() const
{
    switch(_resource.ilwisType()){
    case itFEATURE:
        return new FeatureCoverage(_resource);
    case itRASTER:
        return new RasterCoverage(_resource);
    case itTABLE:
        return new FlatTable(_resource);
    default:
        return 0;
    }
}


StreamConnector::StreamConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource,load,options)
{
    QUrlQuery query(source().url());
    QString url =QString(source().url(true).toString(QUrl::RemoveQuery));
    if ( query.hasQueryItem("datasource")){
        QString item = query.queryItemValue("datasource");
        url += "/" + item;
        source().setUrl(url);
    }
}

StreamConnector::~StreamConnector()
{

}

bool StreamConnector::loadMetaData(IlwisObject *object, const IOOptions &options)
{
    if ( _resource.url(true).scheme() == "file"){
        VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
        if (!openSource(false))
            return false;
        QDataStream stream(_datasource.get());
        IlwisTypes tp;
        QString version;
        stream >> tp;
        stream >> version;
        std::unique_ptr<VersionedSerializer> serializer(factory->create(version,tp,stream));
        if (!serializer)
            return false;
        serializer->connector(this);
        return serializer->loadMetaData(object,options);

    }else {
        DownloadManager manager(_resource);
        return manager.loadMetaData(object,options);
    }

}

bool StreamConnector::loadData(IlwisObject *object, const IOOptions &options){
    if ( _resource.url(true).scheme() == "file"){
        //QFileInfo inf(_resource.url().toLocalFile());
        VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
        QDataStream stream(_datasource.get());
        IlwisTypes tp;
        QString version;
        stream >> tp;
        stream >> version;
        std::unique_ptr<VersionedSerializer> serializer(factory->create(version, tp,stream));
        if (!serializer)
            return false;
        serializer->connector(this);
        _binaryIsLoaded = serializer->loadData(object,options);
        return _binaryIsLoaded;

    }else {
        DownloadManager manager(_resource);
        return manager.loadData(object,options);
    }
}

bool StreamConnector::openSource(bool reading){
    QUrl url = _resource.url(true);
    if ( url.scheme() == "http")
    {
        _bytes.resize(STREAMBLOCKSIZE);
        _bytes.fill(0);
        QBuffer *buf = new QBuffer(&_bytes);
        if (!buf->open(QIODevice::ReadWrite))
            return false;
        _datasource.reset(buf);
        return true;
    }
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


bool StreamConnector::store(IlwisObject *obj, const IOOptions &options){
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
    _versionedConnector->connector(this);
    bool ok;
    int storemode = options.contains("storemode") ? options["storemode"].toInt() : IlwisObject::smMETADATA | IlwisObject::smBINARYDATA;
    if ( hasType(storemode, IlwisObject::smMETADATA))
        ok = _versionedConnector->store(obj, options);
    if ( hasType(storemode, IlwisObject::smBINARYDATA))
        ok = _versionedConnector->storeData(obj, options);

    flush(true);

    _datasource->close();
    _versionedConnector.reset(0);

    return ok;

}

QString StreamConnector::provider() const
{
    return "Stream";
}

bool StreamConnector::needFlush() const
{
    return _datasource->pos() >= STREAMBLOCKSIZE && _resource.url().scheme() != "file";
}

quint32 StreamConnector::position() const {
    if ( _datasource)
        return _datasource->pos();
    return iUNDEF;
}

void StreamConnector::flush(bool last)
{
    if ( _resource.url().scheme() == "file") // we dont flush with files; OS does this
        return;
    QBuffer *buf = static_cast<QBuffer *>(_datasource.get());
    emit dataAvailable(buf,last);

}

bool StreamConnector::isReadOnly() const
{
    return false;
}

bool StreamConnector::isFileBased() const
{
    return source().url(true).scheme() == "file";
}
