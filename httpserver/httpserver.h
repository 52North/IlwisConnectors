#ifndef HTTPSERVER_H
#define HTTPSERVER_H

class HttpListener;

namespace Ilwis {
namespace HTTP {
class HTTPServer : public OperationImplementation
{
public:
    HTTPServer();
    HTTPServer(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~HTTPServer() ;
    static quint64 createMetadata();
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable& symTable);
    bool execute(ExecutionContext *ctx, SymbolTable& symTable);

    NEW_OPERATION(HTTPServer);

private:
    std::unique_ptr<HttpListener> _server;
};
}
}
#endif // HTTPSERVER_H
