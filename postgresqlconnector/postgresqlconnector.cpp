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

PostgresqlConnector::PostgresqlConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource,load,options)
{
}

PostgresqlConnector::~PostgresqlConnector()
{
}

QString PostgresqlConnector::provider() const
{
    return "postgresql";
}

