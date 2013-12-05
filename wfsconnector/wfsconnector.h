#ifndef WFSCONNECTOR_H
#define WFSCONNECTOR_H

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "wfsConnector_global.h"


namespace Ilwis {

namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsConnector : public Ilwis::IlwisObjectConnector {


public:
    WfsConnector(const Ilwis::Resource &resource, bool load=true);
    static IlwisTypes ilwisType(const QString &name);
    QString provider() const;

protected:
    bool loadMetaData(IlwisObject* data);
    bool storeMetaData(const IlwisObject* data, IlwisTypes type) const;
    bool loadBinaryData(IlwisObject* ) { return false; }

private:
    QNetworkAccessManager* _networkManager;


private slots:
    void finishedSlot(QNetworkReply* reply);
};
}

}

#endif // WFSCONNECTOR_H
