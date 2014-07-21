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
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "connectorfactory.h"

#include "postgresqlconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqlobjectfactory.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlObjectFactory::PostgresqlObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","postgresql","")
{
}


bool PostgresqlObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() != "postgresql")
        return false; // can't use anything other than pg connection

    IlwisTypes type = resource.ilwisType() ;
//    if ( type & itDOMAIN)
//        return true;
//    else if ( type & itCOORDSYSTEM)
//        return true;
//    else if ( type & itRASTER)
//        return true;
//    else if ( type & itGEOREF)
//        return true;
//    else if ( type & itFEATURE)
//        return true;
//    else
    if ( type & itTABLE)
        return true;
    return false;
}

IlwisObject *PostgresqlObjectFactory::create(const Resource &resource, const PrepareOptions &options) const
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    PostgresqlConnector *connector;
    IlwisTypes type = resource.ilwisType();
    if (type & itTABLE) {
        connector = factory->createFromResource<PostgresqlTableConnector>(resource, "postgresql", options);
    }
//    else if(type & itFEATURE) {
//          connector = factory->createFromResource<PostgresqlFeatureConnector>(resource, "postgresql", options);
//    }
    else {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
    }
    IlwisObject *object = createObject(connector);
    if ( object)
       return object;

    delete connector;
    return 0;
}
