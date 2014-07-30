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

#include "postgresqlconnector.h"
#include "postgresqltableloader.h"
#include "postgresqlfeatureconnector.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlFeatureConnector::PostgresqlFeatureConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
}

IlwisObject *PostgresqlFeatureConnector::create() const
{
    return new FlatTable(_resource);
}

ConnectorInterface *PostgresqlFeatureConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlFeatureConnector(resource, load,options);
}

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{

    qDebug() << "PostgresqlFeatureConnector::loadMetaData()";
    if ( !PostgresqlConnector::loadMetaData(data, options)) {
        return false;
    }

    QUrl url = source().url();
    ITable *table = (ITable *)data;
    PostgresqlTableLoader loader;
    loader.loadMetadata(table,source());
    table->ptr()->name(url.path());

    // TODO feature count, further metadata.

    return true;
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
