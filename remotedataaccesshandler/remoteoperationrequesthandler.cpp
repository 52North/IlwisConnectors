#include "remoteoperationrequesthandler.h"
#include "juliantime.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"

using namespace Ilwis;
using namespace RemoteDataAccess;


RemoteOperationRequestHandler::RemoteOperationRequestHandler()
{
}

void RemoteOperationRequestHandler::service(HttpRequest &request, HttpResponse &response)
{
    try{
    QMultiMap<QByteArray,QByteArray> parameters = request.getParameterMap();
    QMultiMap<QByteArray,QByteArray>::Iterator iter;
    if ( (iter = parameters.find("expression")) != parameters.end() ){
        QString expr = iter.value();
        OperationExpression operationexpr(expr);
        quint64 operationid = commandhandler()->findOperationId(operationexpr);
        if ( operationid != i64UNDEF){
            Resource metadata = mastercatalog()->id2Resource(operationid);
            QString operationname = metadata.name();
            QString outputs = metadata["outparameters"].toString();
            QStringList parts = outputs.split("|");
            QString outputNames;
            for(int i = 0; i < parts.size(); ++i){
                if ( outputNames != "")
                    outputNames += ",";
                quint64 mark = (quint64)(1e8 * (double)Time::now());
                QString outputName = QString("%1_%2_%3").arg(operationname).arg(i).arg(mark);
                outputNames += outputName;
            }
            QString operationexpression = outputNames + "=" + expr;
            OperationExpression a(operationexpression);
            Operation localoperation(a);
            ExecutionContext ctx;
            SymbolTable tbl;
            localoperation->execute(&ctx, tbl);


        }
    }
    } catch(ErrorObject& err){
        error(err.message(),response);
    }
}

HttpRequestHandler *RemoteOperationRequestHandler::create()
{
    return new RemoteOperationRequestHandler();
}
