#ifndef WFSFEATURECONNECTOR_H
#define WFSFEATURECONNECTOR_H

#include "wfsconnector_global.h"

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsFeatureConnector : public WfsConnector
{
public:
    static ConnectorInterface *create(const Resource &resource, bool load=true, const PrepareOptions &options=PrepareOptions());
    WfsFeatureConnector(const Resource &resource, bool load=true, const PrepareOptions &options=PrepareOptions());

    IlwisObject *create() const;
    static IlwisTypes ilwisType(const QString &name);

    bool loadMetaData(IlwisObject* data);
    bool loadBinaryData(IlwisObject* data);


private:
    WfsParsingContext _context;

    void initFeatureTable(ITable &table) const;

};
}
}
#endif // WFSFEATURECONNECTOR_H


