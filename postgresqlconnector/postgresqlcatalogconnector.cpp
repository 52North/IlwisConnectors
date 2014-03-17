#include "kernel.h"
#include "resource.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "postgresqlcatalogconnector.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlCatalogConnector::PostgresqlCatalogConnector(const Ilwis::Resource &resource, bool load) : CatalogConnector(resource, load)
{

}

bool PostgresqlCatalogConnector::loadItems()
{
    QSqlDatabase db;
    return false;
}

bool PostgresqlCatalogConnector::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (resource.url().scheme() != "ilwis:postgresql")
        return false;
    return true;
}

QString PostgresqlCatalogConnector::provider() const
{
    return "postgresql";
}

std::vector<QUrl> PostgresqlCatalogConnector::sources(const QStringList &filter, int options) const
{
    return std::vector<QUrl>();
}

QFileInfo PostgresqlCatalogConnector::toLocalFile(const QUrl &datasource) const
{
    return QFileInfo();
}

ConnectorInterface *PostgresqlCatalogConnector::create(const Ilwis::Resource &resource, bool load)
{
    return new PostgresqlCatalogConnector(resource,  load);
}
