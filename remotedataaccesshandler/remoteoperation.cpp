#include <QEventLoop>
#include "remoteoperation.h"

using namespace Ilwis;
using namespace RemoteDataAccess;

REGISTER_OPERATION(RemoteOperation)

RemoteOperation::RemoteOperation()
{
}

RemoteOperation::RemoteOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool RemoteOperation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    QString ss = _operationRequest.toString();
    QNetworkRequest request(_operationRequest);

    QNetworkReply *reply = kernel()->network().get(request);

    connect(reply, &QNetworkReply::downloadProgress, this, &RemoteOperation::downloadProgress);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &RemoteOperation::error);
    connect(reply, &QNetworkReply::finished, this, &RemoteOperation::finishedData);
    connect(reply, &QNetworkReply::readyRead, this, &RemoteOperation::readReady);

    QEventLoop loop; // waits for request to complete
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    delete reply;

    return true;
}

void RemoteOperation::readReady()
{

    if (QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender())) {
        _bytes.append(reply->readAll());
    }
}

void RemoteOperation::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
}

void RemoteOperation::error(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    if (QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender())) {
        qErrnoWarning("Error!");

        reply->deleteLater();
    }
}

void RemoteOperation::finishedData()
{
}

Ilwis::OperationImplementation *RemoteOperation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RemoteOperation(metaid, expr);
}

Ilwis::OperationImplementation::State RemoteOperation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString expr = _expression.input<QString>(0);
    if ( expr == "")
        return sPREPAREFAILED;

    QString operation = expr.mid(1,expr.size() - 3);
    _operationexpr = OperationExpression(operation);
    QUrl rightside(_operationexpr.toString(true));
    QString path = rightside.path().mid(1);
    _operationRequest = QUrl(QString("http://%1:%2/operation?expression=%3").arg(rightside.host()).arg(rightside.port()).arg(path));



    if (!_operationRequest.isValid())
        return sPREPAREFAILED;

    return sPREPARED;
}

quint64 RemoteOperation::createMetadata()
{
    OperationResource operation({"ilwis://operations/remoteoperation"});
    operation.setSyntax("remoteoperation(expression");
    operation.setDescription(TR("executes remotely an operation on a server machine. results are either passed direcly back to the client or are stored for later retrieval"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itSTRING , TR("expression"),TR("expression to be executed"));
    operation.setOutParameterCount({0});
    operation.setKeywords("server, remote");

    mastercatalog()->addItems({operation});
    return operation.id();
}
