#ifndef PostgresqlConnection_H
#define PostgresqlConnection_H

namespace Ilwis {
namespace Postgresql {

class PostgresqlConnection :public OperationImplementation
{

public:

    PostgresqlConnection(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(PostgresqlConnection);

private:
    QString _host;
    quint32 _port = 5432;
    QString _database;
    QString _schema = "public";
    QString _table = sUNDEF;
    QString _username = sUNDEF;
    QString _password = sUNDEF;
};
}
}

#endif // PostgresqlConnection_H
