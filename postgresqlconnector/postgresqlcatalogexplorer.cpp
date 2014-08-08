#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "resource.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "catalog.h"

#include "postgresqldatabaseutil.h"
#include "postgresqlcatalogexplorer.h"

using namespace Ilwis;
using namespace Postgresql;

REGISTER_CATALOGEXPLORER(PostgresqlCatalogExplorer)

CatalogExplorer *PostgresqlCatalogExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new PostgresqlCatalogExplorer(resource,  options);
}

PostgresqlCatalogExplorer::PostgresqlCatalogExplorer(const Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{
    if (canUse(resource)) {
        // TODO discuss this workaround
        PostgresqlDatabaseUtil::openForResource(source(), "exploreitems");
    }
}

PostgresqlCatalogExplorer::~PostgresqlCatalogExplorer()
{
    QSqlDatabase::removeDatabase("exploreitems");
}

std::vector<Resource> PostgresqlCatalogExplorer::loadItems()
{
    QString schema("public");
    if (source().hasProperty("pg.schema")) {
        schema = source()["pg.schema"].toString();
    }

    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" meta.tablename, count(geom.typname) as hasGeometry ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" pg_catalog.pg_tables AS meta,pg_catalog.pg_type AS geom ");
    sqlBuilder.append(" WHERE ");
    sqlBuilder.append(" meta.schemaname = '").append(schema).append("' ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append(" geom.typname = 'geometry' ");
    sqlBuilder.append(" GROUP BY ");
    sqlBuilder.append(" meta.tablename;");
    //qDebug() << "SQL: " << sqlBuilder;

    std::vector<Resource> resources;
    QSqlDatabase db = QSqlDatabase::database("exploreitems");
    QSqlQuery query = db.exec(sqlBuilder);

    QString parentDatasourceNormalized = source().url().toString();
    parentDatasourceNormalized = !parentDatasourceNormalized.endsWith("/")
            ? parentDatasourceNormalized.append("/")
            : parentDatasourceNormalized;

    while (query.next()) {
        QString tablename = query.value(0).toString();
        bool hasGeometry = query.value(1).toBool();
        if (tablename == "spatial_ref_sys") {
            continue; // skip system table
        }
        QString resourceId = parentDatasourceNormalized;
        resourceId.append(schema);
        resourceId.append("/");
        resourceId.append(tablename);
        //qDebug() << "create new resource: " << resourceId;

        IlwisTypes mainType;
        IlwisTypes extTypes;
        if ( hasGeometry) {
            mainType = itCOVERAGE;
            extTypes = itTABLE; // | itGEOREF;
        } else {
            mainType = itTABLE;
        }

        QUrl url(resourceId);
        Resource table(url, mainType);
        table.setExtendedType(extTypes);
        table.addProperty("pg.user", source()["pg.user"]);
        table.addProperty("pg.password", source()["pg.password"]);
        table.addProperty("pg.schema", source().hasProperty("pg.schema")
                          ? source()["pg.schema"]
                          : "public");

        resources.push_back(table);
    }

    return resources;
}

bool PostgresqlCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (resource.url().scheme() != "postgresql")
        return false;
    return true;
}

QString PostgresqlCatalogExplorer::provider() const
{
    return "postgresql";
}

