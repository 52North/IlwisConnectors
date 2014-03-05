#ifndef WFSCATALOGCONNECTOR_H
#define WFSCATALOGCONNECTOR_H

#include "wfsConnector_global.h"

namespace Ilwis {

namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCatalogConnector : public CatalogConnector
{
public:
    WfsCatalogConnector(const Resource &resource,bool load=true);
    ~WfsCatalogConnector();

    virtual bool loadItems();

    bool canUse(const Resource& resource) const;

    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);




    /*
     * following two methods has to be implemented as defined pure
     *
     * However, they do not have any purpose in the WFS catalog connector
     */
    std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const;
    QFileInfo toLocalFile(const QUrl &datasource) const;

};
}
}

#endif // WFSCATALOGCONNECTOR_H
