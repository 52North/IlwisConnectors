#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "postgresqlconnector.h"


using namespace Ilwis;
using namespace Postgresql;

PostgresqlConnector::PostgresqlConnector(const Ilwis::Resource &resource, bool load, const PrepareOptions &options) : IlwisObjectConnector(resource,load,options)
{
    QString url = resource.url().toString();
    QStringList parts = url.split("/");
    _host = parts[2];
    _database = parts[3];
    QUrlQuery query = resource.urlQuery();
    _user = query.queryItemValue("user");
    _password = query.queryItemValue("password");
}

PostgresqlConnector::~PostgresqlConnector()
{

}

bool PostgresqlConnector::loadMetaData(IlwisObject *data, const PrepareOptions &options)
{
    if (data == nullptr)
        return false;
    return false;

}

QString PostgresqlConnector::provider() const
{
    return "postgresql";
}
