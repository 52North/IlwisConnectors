#ifndef POSTGRESQLTABLECONNECTOR_H
#define POSTGRESQLTABLECONNECTOR_H

namespace Ilwis {
namespace Postgresql {
class PostgresqlTableConnector : public PostgresqlConnector
{
public:
    PostgresqlTableConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());


    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions&);

    bool loadData(IlwisObject *data);
    bool store(Ilwis::IlwisObject *data);

};
}
}

#endif // POSTGRESQLTABLECONNECTOR_H
