#include "kernel.h"
#include "ilwisdata.h"
#include "resource.h"
#include "identity.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "httpserver/httplistener.h"
#include "httpserver.h"

using namespace Ilwis;
using namespace HTTP;

REGISTER_OPERATION(HTTPServer)

OperationImplementation *HTTPServer::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new HTTPServer(metaid, expr);
}

HTTPServer::HTTPServer()
{
}

HTTPServer::HTTPServer(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

HTTPServer::~HTTPServer()
{

}

OperationImplementation::State HTTPServer::prepare(ExecutionContext *ctx, const SymbolTable& symTable) {
    return OperationImplementation::sPREPARED;
}

bool HTTPServer::execute(ExecutionContext *ctx, SymbolTable &)
{

    _server.reset(new HttpListener(0));

    return true;
}

quint64 HTTPServer::createMetadata()
{
    OperationResource operation({"ilwis://operations/httpserver"});
    operation.setSyntax("httpserver([port[,maxhtreads[,cleanupInterval[,readTimeout[,maxRequestSize[,maxMultiPartSize]]]]]]");
    operation.setDescription(TR("transpose the raster according to the method indicated by the second parameter"));
    operation.setInParameterCount({0,1,2,3,4,5});
    operation.addInParameter(0,itUINT16,  TR("port number"));
    operation.addInParameter(1,itUINT16, TR("max number of threads"),TR("maximum number of threads the server is allowed to start"));
    operation.addInParameter(2,itUINT16, TR("cleanup interval"),TR("Time interval for starting the garbage collect"));
    operation.addInParameter(3,itUINT32, TR("read timeout"),TR("Interval after which a read disconnects"));
    operation.addInParameter(4,itUINT32, TR("max request size"));
    operation.addInParameter(5,itUINT16, TR("max  multi part size"));
    operation.setOutParameterCount({0});
    operation.setKeywords("server, http");

    mastercatalog()->addItems({operation});
    return operation.id();
}


