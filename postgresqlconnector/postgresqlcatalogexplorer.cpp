#include "kernel.h"
#include "resource.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "catalog.h"

#include "postgresqlcatalogexplorer.h"

using namespace Ilwis;
using namespace Postgresql;

REGISTER_CATALOGEXPLORER(PostgresqlCatalogExplorer)

CatalogExplorer *PostgresqlCatalogExplorer::create(const Resource &resource, const PrepareOptions &options)
{
    return new PostgresqlCatalogExplorer(resource,  options);
}

PostgresqlCatalogExplorer::PostgresqlCatalogExplorer(const Resource &resource, const PrepareOptions &options) : CatalogExplorer(resource, options)
{
}

std::vector<Resource> PostgresqlCatalogExplorer::loadItems()
{
    QSqlDatabase db;
    std::vector<Resource> resources;

    // TODO load all resources from DB

    return resources;
}

bool PostgresqlCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (resource.url().scheme() != "ilwis:postgresql")
        return false;
    return true;
}

QString PostgresqlCatalogExplorer::provider() const
{
    return "postgresql";
}

