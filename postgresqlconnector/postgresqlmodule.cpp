#include "kernel.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "postgresqlobjectfactory.h"
#include "postgresqlconnector.h"
#include "connectorinterface.h"
#include "postgresqlcatalogconnector.h"
#include "postgresqltableconnector.h"
#include "catalogconnector.h"
#include "ilwiscontext.h"
#include "postgresqlmodule.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlModule::PostgresqlModule(QObject *parent) : Module(parent, "PostgresqlModule", "iv40","1.0")
{

}

PostgresqlModule::~PostgresqlModule()
{

}

QString PostgresqlModule::getInterfaceVersion() const
{
      return "iv40";
}

QString PostgresqlModule::getName() const
{
    return "Postgressql plugin";
}

QString PostgresqlModule::getVersion() const
{
    return "1.0";
}

void PostgresqlModule::prepare()
{
//    QString ilwisfolder = context()->ilwisFolder().absoluteFilePath();
//    QString path = ilwisfolder + "/extensions/postgresqlconnector/libpq";
//    QLibrary lib;
//    lib.setFileName(path);
//    if ( !lib.load()){
//        ERROR2(ERR_COULD_NOT_LOAD_2,"libpq",lib.errorString());
//    }

    PostgresqlObjectFactory *factory = new PostgresqlObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

//    CatalogConnectorFactory *catfact = kernel()->factory<CatalogConnectorFactory>("catalogconnectorfactory", "ilwis");
//    if ( catfact)
//        catfact->add(PostgresqlCatalogConnector::create);

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;

    cfactory->addCreator(itTABLE,"gdal", PostgresqlTableConnector::create);
}
