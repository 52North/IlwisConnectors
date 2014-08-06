#ifndef POSTGRESQLFEATURECONNECTOR_H
#define POSTGRESQLFEATURECONNECTOR_H

namespace Ilwis {

class FeatureCoverage;

namespace Postgresql {
class PostgresqlFeatureConnector : public PostgresqlConnector
{
public:
    PostgresqlFeatureConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
    ~PostgresqlFeatureConnector();


    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions&);
    bool loadData(IlwisObject *data);
    bool store(IlwisObject* data);

};
}
}

#endif // POSTGRESQLFEATURECONNECTOR_H
