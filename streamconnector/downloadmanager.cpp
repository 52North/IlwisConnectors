#include <QEventLoop>
#include "kernel.h"
#include "resource.h"
#include "version.h"
#include "ilwisdata.h"
#include "raster.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"
#include "downloadmanager.h"

using namespace Ilwis;
using namespace Stream;

DownloadManager::DownloadManager(const Resource& resource,QNetworkAccessManager& manager) : _resource(resource), _manager(manager)
{
}

bool DownloadManager::loadData(IlwisObject *object, const IOOptions &options){
    QUrl url = _resource.url(true);

    QUrlQuery query(url);
    if ( object->ilwisType() == itRASTER){
        if ( options.contains("blockindex")){
            RasterCoverage *raster = static_cast<RasterCoverage*>(object);
            int bindex = options["blockindex"].toInt();
            int layer = bindex / raster->grid()->blocksPerBand();
            int relativeBlock = bindex - layer * raster->grid()->blocksPerBand();
            unsigned int minLine = raster->grid()->maxLines() * relativeBlock ;
            unsigned int maxLine = std::min( minLine + raster->grid()->maxLines(), raster->size().ysize());
            query.addQueryItem("lines",QString("%1 %2 %3").arg(layer).arg(minLine).arg(maxLine));
        }
    }
    query.addQueryItem("datatype","data");
    url.setQuery(query);
    QString urltxt = url.toString();
    _object = object;
    QNetworkRequest request(url);

    QNetworkReply *reply = _manager.get(request);

    connect(reply, &QNetworkReply::readyRead, this, &DownloadManager::readReady);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadManager::downloadProgress);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &DownloadManager::error);
    connect(reply, &QNetworkReply::finished, this, &DownloadManager::finishedData);

    QEventLoop loop; // waits for request to complete
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    delete reply;

    return true;
}

bool DownloadManager::loadMetaData(IlwisObject *object, const IOOptions &options)
{
    QUrl url = _resource.url(true);
    QUrlQuery query(url);
    query.addQueryItem("datatype","metadata");
    url.setQuery(query);
    _object = object;
    QString urltxt = url.toString();
    QNetworkRequest request(url);

    QNetworkReply *reply = _manager.get(request);

    connect(reply, &QNetworkReply::readyRead, this, &DownloadManager::readReady);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadManager::downloadProgress);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &DownloadManager::error);
    connect(reply, &QNetworkReply::finished, this, &DownloadManager::finishedMetadata);

    QEventLoop loop; // waits for request to complete
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    delete reply;

    return true;
}

void DownloadManager::readReady()
{

    if (QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender())) {
        _bytes.append(reply->readAll());
    }
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
}

void DownloadManager::error(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    if (QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender())) {
        qErrnoWarning("Error!");

        reply->deleteLater();
    }
}

void DownloadManager::finishedData()
{
    QBuffer buf(&_bytes);
    buf.open(QIODevice::ReadWrite);
    QDataStream stream(&buf);
    quint64 type;
    stream >> type;
    QString version;
    stream >> version;

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (factory){
        _versionedConnector.reset( factory->create(version,type,stream));
    }

    if (!_versionedConnector)
        return ;
    _versionedConnector->loadData(_object, IOOptions());

    buf.close();
}
void DownloadManager::finishedMetadata()
{
    QBuffer buf(&_bytes);
    buf.open(QIODevice::ReadWrite);
    QDataStream stream(&buf);
    quint64 type;
    stream >> type;
    QString version;
    stream >> version;

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (factory){
        _versionedConnector.reset( factory->create(version,type,stream));
    }

    if (!_versionedConnector)
        return ;
    _versionedConnector->loadMetaData(_object, IOOptions());

    buf.close();
}

