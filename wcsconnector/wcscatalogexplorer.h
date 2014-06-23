#ifndef WFSCATALOGCONNECTOR_H
#define WFSCATALOGCONNECTOR_H


namespace Ilwis {

namespace Wcs {

class WcsCatalogExplorer : public CatalogExplorer
{
public:

    WcsCatalogExplorer(const Resource &resource, const PrepareOptions &options=PrepareOptions());
    ~WcsCatalogExplorer();

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

    NEW_CATALOGEXPLORER(WcsCatalogExplorer);

};
}
}

#endif // WFSCATALOGCONNECTOR_H
