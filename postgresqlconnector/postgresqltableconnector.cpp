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
#include "flattable.h"

#include "postgresqldatabaseutil.h"
#include "postgresqlconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqltableloader.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlTableConnector::PostgresqlTableConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
}

IlwisObject *PostgresqlTableConnector::create() const
{
    qDebug() << "PostgresqlTableConnector::create()";
    return new FlatTable(_resource);
}

ConnectorInterface *PostgresqlTableConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlTableConnector(resource, load,options);
}

bool PostgresqlTableConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    qDebug() << "PostgresqlTableConnector::loadMetaData()";
    if ( !PostgresqlConnector::loadMetaData(data, options)) {
        return false;
    }

    Table *table = static_cast<Table *>(data);
    PostgresqlTableLoader loader;
    loader.loadMetadata(table,source());

    // TODO feature count, further metadata.


    return true;
}

bool PostgresqlTableConnector::storeMetaData(Ilwis::IlwisObject *obj)
{
    qDebug() << "PostgresqlTableConnector::storeMetaData()";
    return false;
}

bool PostgresqlTableConnector::loadBinaryData(IlwisObject *data)
{
    qDebug() << "PostgresqlTableConnector::loadBinaryData()";
    return false;
}

bool PostgresqlTableConnector::storeBinaryData(IlwisObject *obj)
{
    qDebug() << "PostgresqlTableConnector::storeBinaryData()";
    return false;
}
