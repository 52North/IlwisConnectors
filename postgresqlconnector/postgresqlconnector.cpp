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
    int index = resource.url(true).toString().indexOf("postgresql://");
    if ( index == 0) {
        QString rest = resource.url(true).toString().mid(13);
        QStringList parts = rest.split("/");
        QStringList hostport = parts[0].split(":");
        ioOptionsRef().addOption("port", hostport[1]);
        ioOptionsRef().addOption("host", hostport[0]);
        ioOptionsRef().addOption("database", parts[1]);
        ioOptionsRef().addOption("pg.schema", parts[2]);
        QStringList userpass = parts[3].split("&");
        ioOptionsRef().addOption("pg.username", userpass[0].split("=")[1]);
        ioOptionsRef().addOption("pg.password", userpass[1].split("=")[1]);

    }
}

PostgresqlConnector::~PostgresqlConnector()
{
}

QString PostgresqlConnector::provider() const
{
    return "postgresql";
}

