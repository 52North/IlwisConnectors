#ifndef WFSCONNECTOR_H
#define WFSCONNECTOR_H

#include <QObject>

#include "wfsconnector_global.h"
#include "kernel.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"

class QString;
class QNetworkReply;
class QNetworkAccessManager;

namespace Ilwis {

namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsConnector : public QObject, public Ilwis::IlwisObjectConnector {

    Q_OBJECT

public:
    WfsConnector(const Ilwis::Resource &resource, bool load=true);

    static IlwisTypes ilwisType(const QString &name);
    QString provider() const;

protected:
    bool loadMetaData(IlwisObject* data) { return false; }
    bool loadBinaryData(IlwisObject* ) { return false; }

};
}

}

#endif // WFSCONNECTOR_H
