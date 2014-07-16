#include <QtPlugin>
#include <QSqlQuery>
#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "symboltable.h"
#include "gatherermodule.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "gathererrequesthandler.h"
#include "httpserver/requesthandlerfactory.h"




using namespace Ilwis;
using namespace Gatherer;

GathererModule::GathererModule(QObject *parent) :
    Module(parent, "GathererModule", "iv40","1.0")
{
}

QString GathererModule::getInterfaceVersion() const
{
    return "iv40";

}

QString GathererModule::getName() const
{
    return "Gatherer plugin";
}

QString GathererModule::getVersion() const
{
    return "1.0";
}

void GathererModule::finalizePreparation()
{
    HTTP::HTTPRequestHandlerFactory *factory = kernel()->factory<HTTP::HTTPRequestHandlerFactory>("ilwis::HTTPRequestHandlerFactory");
    if ( factory){
        factory->addHandler("/gatherer", GathererRequestHandler::create);
    }
}




