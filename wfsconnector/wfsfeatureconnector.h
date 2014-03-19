#ifndef WFSFEATURECONNECTOR_H
#define WFSFEATURECONNECTOR_H

#include <QObject>

#include "wfsconnector_global.h"

#include "kernel.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"

#include "wfsparsingcontext.h"

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsFeatureConnector : public QObject, public IlwisObjectConnector
{
    Q_OBJECT

public:
    static ConnectorInterface *create(const Resource &resource, bool load=true, const PrepareOptions &options=PrepareOptions());
    WfsFeatureConnector(const Resource &resource, bool load=true, const PrepareOptions &options=PrepareOptions());

    QString provider() const;
    IlwisObject *create() const;
    static IlwisTypes ilwisType(const QString &name);

protected:
    bool loadMetaData(IlwisObject* data);
    bool loadBinaryData(IlwisObject* data);

private:
    WfsParsingContext _context;
};
}
}
#endif // WFSFEATURECONNECTOR_H


