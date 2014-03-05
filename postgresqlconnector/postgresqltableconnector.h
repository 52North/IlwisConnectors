#ifndef POSTGRESQLTABLECONNECTOR_H
#define POSTGRESQLTABLECONNECTOR_H

namespace Ilwis {
namespace Postgresql {
class PostgresqlTableConnector : public PostgresqlConnector
{
public:
    PostgresqlTableConnector(const Ilwis::Resource &resource, bool load);


    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load);

    bool loadMetaData(IlwisObject* data);
    bool storeMetaData(Ilwis::IlwisObject *obj);

    bool loadBinaryData(IlwisObject *data);
    bool storeBinaryData(IlwisObject* obj);
};
}
}

#endif // POSTGRESQLTABLECONNECTOR_H
