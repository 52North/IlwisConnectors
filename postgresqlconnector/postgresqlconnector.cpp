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
}

PostgresqlConnector::~PostgresqlConnector()
{
}

bool PostgresqlConnector::loadMetaData(IlwisObject *data, const PrepareOptions &options)
{
//    if (data == nullptr)
//        return false;
//    return false;
    return true;
}

QString PostgresqlConnector::provider() const
{
    return "postgresql";
}

