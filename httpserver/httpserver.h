#ifndef HTTPSERVER_H
#define HTTPSERVER_H

namespace Ilwis {
namespace HTTP {
class HTTPServer : public OperationImplementation
{
public:
    HTTPServer();
    HTTPServer(quint64 metaid, const Ilwis::OperationExpression &expr);
    static quint64 createMetadata();
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    OperationImplementation::State prepare();
    bool execute(ExecutionContext *ctx);
};
}
}
#endif // HTTPSERVER_H
