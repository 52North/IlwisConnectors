#include "remoteoperationrequesthandler.h"
#include "raster.h"
#include "table.h"
#include "featurecoverage.h"
#include "juliantime.h"
#include "symboltable.h"
#include "ilwiscontext.h"
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
            if ( localoperation->execute(&ctx, tbl)){
                for(auto result : ctx._results){
                    Symbol sym = tbl.getSymbol(result);
                    QString internalUrl = context()->persistentInternalCatalog().toString() + "/" + result + ".ilwis4";
                    QString format;
                    IIlwisObject obj;
                    if ( hasType(sym._type, itFEATURE)){
                        obj = sym._var.value<Ilwis::IFeatureCoverage>();
                        format = "featurecoverage";
                    }else if ( sym._type == itRASTER){
                        obj = sym._var.value<Ilwis::IRasterCoverage>();
                        format = "rastercoverage";
                    }else if (hasType(sym._type, itTABLE)){
                        obj = sym._var.value<Ilwis::ITable>();
                        format = "table";
                    }else if (hasType(sym._type, itCATALOG)){
                        obj = sym._var.value<Ilwis::ICatalog>();
                        format = "catalog";
                    }else
                        continue;

                    obj->connectTo(internalUrl,format,"stream",IlwisObject::cmOUTPUT);
                    obj->store();
                }
            }


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
