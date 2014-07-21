#ifndef POSTGRESQLCONNECTOR_H
#define POSTGRESQLCONNECTOR_H

namespace Ilwis {
namespace Postgresql {

class PostgresqlConnector : public IlwisObjectConnector
{
public:
    PostgresqlConnector(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
    virtual ~PostgresqlConnector();

    virtual bool loadMetaData(IlwisObject* data,const PrepareOptions&);

    QString provider() const;

private:

};
}
}

#endif // POSTGRESQLCONNECTOR_H
