#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDataStream>
#include "ilwis.h"
#include "identity.h"
#include "resource.h"
#include "iooptions.h"
#include "connectorinterface.h"
#include "versionedserializer.h"

namespace Ilwis {

class IlwisObject;
class IOOptions;

namespace Stream {

class VersionedSerializer;


class DownloadManager : public QObject
{
    Q_OBJECT
public:
    DownloadManager(const Ilwis::Resource &resource, QNetworkAccessManager &manager);
    bool loadMetaData(IlwisObject *object, const IOOptions &options);
    bool loadData(IlwisObject *object, const IOOptions &options);

public slots:
protected slots:
    void readReady();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void finishedMetadata();
    void finishedData();

private:
    Resource _resource;
    QNetworkAccessManager& _manager;
    std::unique_ptr<VersionedSerializer> _versionedConnector;
    QByteArray _bytes;
    IlwisObject *_object;
};
}
}

#endif // DOWNLOADMANAGER_H
