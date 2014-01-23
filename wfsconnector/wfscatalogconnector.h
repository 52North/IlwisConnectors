#ifndef WFSCATALOGCONNECTOR_H
#define WFSCATALOGCONNECTOR_H

#include "wfsConnector_global.h"

namespace Ilwis {

namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCatalogConnector : public CatalogConnector
{
public:
    WfsCatalogConnector(const Ilwis::Resource &resource);
    ~WfsCatalogConnector();

    virtual bool loadItems();

    bool canUse(const Resource& resource) const;

    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool);

private:
    bool isValidWfsUrl(QUrl url) const;
    bool isExpectedValue(QString actual, QString value) const;
    QList<QPair<QString,QString>> lowerCaseKeys(QList<QPair<QString,QString>> queryItems) const;
};
}
}

#endif // WFSCATALOGCONNECTOR_H
