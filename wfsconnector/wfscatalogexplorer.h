#ifndef WFSCATALOGCONNECTOR_H
#define WFSCATALOGCONNECTOR_H

#include "wfsConnector_global.h"

namespace Ilwis {

namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCatalogExplorer : public CatalogExplorer
{
public:

    WfsCatalogExplorer(const Resource &resource, const PrepareOptions &options=PrepareOptions());
    ~WfsCatalogExplorer();

    static Ilwis::CatalogExplorer *create(const Resource &resource,const PrepareOptions& options=PrepareOptions());

    virtual std::vector<Resource> loadItems();

    bool canUse(const Resource& resource) const;

    QString provider() const;





    /*
     * following method(s) has to be implemented as defined pure
     *
     * However, they do not have any purpose in the WFS catalog connector
     */
    QFileInfo toLocalFile(const QUrl &datasource) const;

private:

    NEW_CATALOGEXPLORER(WfsCatalogExplorer);

};
}
}

#endif // WFSCATALOGCONNECTOR_H
