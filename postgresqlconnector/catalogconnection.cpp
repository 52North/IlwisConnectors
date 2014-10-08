#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "catalogconnection.h"

using namespace Ilwis;
using namespace Stream;

    REGISTER_OPERATION(CatalogConnection)

CatalogConnection::CatalogConnection(quint64 metaid, const Ilwis::OperationExpression &expr)
{

}

bool CatalogConnection::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    QString expression;
    if (_schema.isEmpty()) {
        expression = QString("postgresql://%1:%2/%3/public?").arg(_host).arg(_port).arg(_database);
    } else {
        expression = QString("postgresql://%1:%2/%3/%4?").arg(_host).arg(_port).arg(_database).arg(_schema);
    }

    if ( _username != "" && _password != ""){
        expression += QString("username=%1&password=%2").arg(_username).arg(_password);
    }

    QVariant value;
    value.setValue<QString>(expression);
    ctx->setOutput(symTable, value, sUNDEF, itSTRING, Resource());

    return true;
}

Ilwis::OperationImplementation *CatalogConnection::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return  new CatalogConnection(metaid, expr);
}

Ilwis::OperationImplementation::State CatalogConnection::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    _host = _expression.input<QString>(0);
    _port = _expression.input<quint32>(1);
    _database = _expression.input<QString>(2);
    _schema = _expression.input<QString>(3);
    _username = _expression.input<QString>(4);
    _password = _expression.input<QString>(5);
    if ( _host == "" || _port == 0)
        return sPREPAREFAILED;

    return sPREPARED;
}

quint64 CatalogConnection::createMetadata()
{
    OperationResource operation({"ilwis://operations/postgresqlcatalog"});
    operation.setSyntax("postgresqlcatalog(host, port, database, schema, username, password)");
    operation.setDescription(TR("creates a url to access the catalog of a postgresql/postgis database"));
    operation.setInParameterCount({6});
    operation.addInParameter(0,itSTRING , TR("host address"),TR("identifies the host that is running the ilwis server"));
    operation.addInParameter(1,itPOSITIVEINTEGER , TR("port number"),TR("port used on the remote server"));
    operation.addInParameter(2,itSTRING , TR("database"),TR("the database to connect to"));
    operation.addInParameter(3,itSTRING , TR("schema"),TR("schema where tables lay in, default is public"));
    operation.addInParameter(4,itSTRING , TR("username"),TR("username for authentication on the remote server"));
    operation.addInParameter(5,itSTRING , TR("password"),TR("password for authentication on the remote server"));
    operation.setOutParameterCount({1,2});
    operation.addOutParameter(0,itSTRING, TR("connection url"),TR("string that is sufficient to access the remote catalog"));
    operation.setKeywords("service,postgresql,postgis,database");

    mastercatalog()->addItems({operation});
    return operation.id();
}
