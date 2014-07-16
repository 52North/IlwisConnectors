#include <QSqlQuery>
#include <QSqlError>
#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "databasetable.h"

#include "postgresqlconnector.h"
#include "postgresqlfeatureconnector.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlFeatureConnector::PostgresqlFeatureConnector(const Ilwis::Resource &resource, bool load, const PrepareOptions &options) : PostgresqlConnector(resource, load,options)
{
}

IlwisObject *PostgresqlFeatureConnector::create() const
{
    return new DatabaseTable(_resource);
}

ConnectorInterface *PostgresqlFeatureConnector::create(const Ilwis::Resource &resource, bool load,const PrepareOptions& options)
{
    return new PostgresqlFeatureConnector(resource, load,options);
}

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const PrepareOptions &options)
{
    return PostgresqlConnector::loadMetaData(data, options);
}

bool PostgresqlFeatureConnector::storeMetaData(Ilwis::IlwisObject *obj)
{
    return false;
}

bool PostgresqlFeatureConnector::loadBinaryData(IlwisObject *data)
{
    return false;
}

bool PostgresqlFeatureConnector::storeBinaryData(IlwisObject *obj)
{
    return false;
}
