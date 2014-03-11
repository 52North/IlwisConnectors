#ifndef WFSFEATURECONNECTOR_H
#define WFSFEATURECONNECTOR_H

#include "wfsconnector_global.h"

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsFeatureConnector : public WfsConnector
{
public:
    WfsFeatureConnector(const Resource &resource, bool load=true);

    IlwisObject *create() const;
    static IlwisTypes ilwisType(const QString &name);
    static ConnectorInterface *create(const Resource &resource, bool load=true);

    bool loadMetaData(IlwisObject* data);
    bool loadBinaryData(IlwisObject* data);


private:
    WfsParsingContext _context;

    void initFeatureTable(ITable &table) const;

};
}
}
#endif // WFSFEATURECONNECTOR_H


