#ifndef POSTGRESQLCATALOGEXPLORER_H
#define POSTGRESQLCATALOGEXPLORER_H

namespace Ilwis {
namespace Postgresql {


class PostgresqlCatalogExplorer : public CatalogExplorer
{
public:
    PostgresqlCatalogExplorer(const Resource &resource, const PrepareOptions &options=PrepareOptions());

    static CatalogExplorer *create(const Resource &resource,const PrepareOptions& options=PrepareOptions());

    virtual std::vector<Resource> loadItems();

    bool canUse(const Resource& resource) const;

    QString provider() const;

    /*
     * does not have any purpose in the WFS catalog connector
     */
    QFileInfo toLocalFile(const QUrl& ) const {
        QFileInfo fileInfo;
        return fileInfo;
    }

private:

    NEW_CATALOGEXPLORER(PostgresqlCatalogExplorer);
};
}
}

#endif // POSTGRESQLCATALOGEXPLORER_H
