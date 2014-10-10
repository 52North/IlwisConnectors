#include "remoteoperationrequesthandler.h"

using namespace Ilwis;
using namespace RemoteDataAccess;


RemoteOperationRequestHandler::RemoteOperationRequestHandler()
{
}

void RemoteOperationRequestHandler::service(HttpRequest &request, HttpResponse &response)
{
    QMultiMap<QByteArray,QByteArray> parameters = request.getParameterMap();
    QMultiMap<QByteArray,QByteArray>::Iterator iter;
    if ( (iter = parameters.find("expression")) != parameters.end() ){
    }
}

HttpRequestHandler *RemoteOperationRequestHandler::create()
{
    return new RemoteOperationRequestHandler();
}
