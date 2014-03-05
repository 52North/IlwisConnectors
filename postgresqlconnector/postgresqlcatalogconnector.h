#ifndef POSTGRESQLCATALOGCONNECTOR_H
#define POSTGRESQLCATALOGCONNECTOR_H

namespace Ilwis {
namespace Postgresql {
class PostgresqlCatalogConnector : public CatalogConnector
{
public:
    PostgresqlCatalogConnector(const Ilwis::Resource &resource, bool load);

    virtual bool loadItems();
    bool canUse(const Resource& resource) const;
    QString provider() const;
    std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const;
    QFileInfo toLocalFile(const QUrl& datasource) const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
};
}
}

#endif // POSTGRESQLCATALOGCONNECTOR_H
