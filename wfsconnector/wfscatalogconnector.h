#ifndef WFSCATALOGCONNECTOR_H
#define WFSCATALOGCONNECTOR_H

class QUrl;

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

private:
    bool isValidWfsUrl(QUrl url) const;
};
}
}

#endif // WFSCATALOGCONNECTOR_H
