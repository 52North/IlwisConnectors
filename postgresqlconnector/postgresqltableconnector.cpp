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
#include "postgresqltableconnector.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlTableConnector::PostgresqlTableConnector(const Ilwis::Resource &resource, bool load, const PrepareOptions &options) : PostgresqlConnector(resource, load,options)
{
}

IlwisObject *PostgresqlTableConnector::create() const
{
    return new DatabaseTable(_resource);
}

ConnectorInterface *PostgresqlTableConnector::create(const Ilwis::Resource &resource, bool load,const PrepareOptions& options)
{
    return new PostgresqlTableConnector(resource, load,options);
}

bool PostgresqlTableConnector::loadMetaData(IlwisObject *data, const PrepareOptions &options)
{
    qDebug() << "PostgresqlTableConnector::loadMetaData()";

    return PostgresqlConnector::loadMetaData(data, options);
}

bool PostgresqlTableConnector::storeMetaData(Ilwis::IlwisObject *obj)
{
    return false;
}

bool PostgresqlTableConnector::loadBinaryData(IlwisObject *data)
{
    return false;
}

bool PostgresqlTableConnector::storeBinaryData(IlwisObject *obj)
{
    return false;
}
