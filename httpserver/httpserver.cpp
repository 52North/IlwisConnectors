#include "kernel.h"
#include "ilwisdata.h"
#include "resource.h"
#include "identity.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "httpserver.h"

using namespace Ilwis;
using namespace HTTP;

OperationImplementation *HTTPServer::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new HTTPServer(metaid, expr);
}

HTTPServer::HTTPServer()
{
}

HTTPServer::HTTPServer::HTTPServer(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

OperationImplementation::State HTTPServer::prepare() {
    return OperationImplementation::sPREPARED;
}

bool HTTPServer::execute(ExecutionContext *ctx)
{
    return false;
}

quint64 HTTPServer::createMetadata()
{
    QString urlTxt = QString("ilwis://operations/startserver");
    QUrl url(urlTxt);
    Resource res(url, itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","startserver");
    res.addProperty("syntax","startserver");

    res.prepare();
    urlTxt += "=" + QString::number(res.id());
    res.setUrl(QUrl(urlTxt));

    mastercatalog()->addItems({res});
    return res.id();
}


