#include <QEventLoop>
#include "kernel.h"
#include "resource.h"
#include "version.h"
#include "ilwisdata.h"
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

bool DownloadManager::loadMetaData(IlwisObject *object, const IOOptions &options)
{
    QUrl url = _resource.url(true);
    _object = object;
    QNetworkRequest request(url);
    //request.setAttribute(QNetworkRequest::User, filename);
    // request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    QNetworkReply *reply = _manager.get(request);

    connect(reply, &QNetworkReply::readyRead, this, &DownloadManager::readReady);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadManager::downloadProgress);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &DownloadManager::error);
    connect(reply, &QNetworkReply::finished, this, &DownloadManager::finished);

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

void DownloadManager::finished()
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

