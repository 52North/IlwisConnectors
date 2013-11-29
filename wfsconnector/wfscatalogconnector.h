#ifndef WFSCATALOGCONNECTOR_H
#define WFSCATALOGCONNECTOR_H

namespace Ilwis {

namespace Wfs {

class WfsCatalogConnector : public CatalogConnector
{
public:
    WfsCatalogConnector(const Ilwis::Resource &resource);

    virtual bool loadItems();

    bool canUse(const Resource& resource) const;

    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool);

};
}
}

#endif // WFSCATALOGCONNECTOR_H
