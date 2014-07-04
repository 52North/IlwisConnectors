#include <QtPlugin>
#include <QSqlQuery>
#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "symboltable.h"
#include "HttpServerModule.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "httpserver.h"


using namespace Ilwis;
using namespace HTTP;

HttpServerModule::HttpServerModule(QObject *parent) :
    Module(parent, "HttpServerModule", "iv40","1.0")
{
}

QString HttpServerModule::getInterfaceVersion() const
{
    return "iv40";

}

void HttpServerModule::prepare()
{
}

QString HttpServerModule::getName() const
{
    return "HTTP Server plugin";
}

QString HttpServerModule::getVersion() const
{
    return "1.0";
}




