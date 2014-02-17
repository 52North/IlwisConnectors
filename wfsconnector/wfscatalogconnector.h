#ifndef WFSCATALOGCONNECTOR_H
#define WFSCATALOGCONNECTOR_H

#include "wfsConnector_global.h"

namespace Ilwis {

namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCatalogConnector : public CatalogConnector
{
public:
    WfsCatalogConnector(const Resource &resource);
    ~WfsCatalogConnector();

    virtual bool loadItems();

    bool canUse(const Resource& resource) const;

    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool);

};
}
}

#endif // WFSCATALOGCONNECTOR_H
