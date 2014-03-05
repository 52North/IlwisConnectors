#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QHash>

#include "kernel.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "connectorfactory.h"
#include "postgresqlconnector.h"
#include "postgresqlobjectfactory.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlObjectFactory::PostgresqlObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","postgresql","")
{
}


bool PostgresqlObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() != "ilwis:postgresql") // can't use anything marked as internal
        return false;

    IlwisTypes type = resource.ilwisType() ;
    if ( type & itDOMAIN)
        return true;
    else if ( type & itCOORDSYSTEM)
        return true;
    else if ( type & itRASTER)
        return true;
    else if ( type & itGEOREF)
        return true;
    else if ( type & itFEATURE)
        return true;
    else if ( type & itTABLE)
        return true;
}

IlwisObject *PostgresqlObjectFactory::create(const Resource &resource) const
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    PostgresqlConnector *connector = factory->createFromResource<PostgresqlConnector>(resource, "postgresql");

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
   IlwisObject *object = createObject(connector);
   if ( object)
       return object;

   delete connector;
   return 0;
}
