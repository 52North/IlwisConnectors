#ifndef POSTGRESQLFEATURECONNECTOR_H
#define POSTGRESQLFEATURECONNECTOR_H

namespace Ilwis {

class FeatureCoverage;

namespace Postgresql {
class PostgresqlFeatureConnector : public PostgresqlConnector
{
public:
    PostgresqlFeatureConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());


    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions&);
    bool storeMetaData(Ilwis::IlwisObject *obj);

    bool loadBinaryData(IlwisObject *data);
    bool storeBinaryData(IlwisObject* obj);
private:
    bool prepareTableForFeatureCoverage(FeatureCoverage *fcoverage) const;
    void setFeatureCount(FeatureCoverage *fcoverage) const;
    void setEnvelope(FeatureCoverage *fcoverage) const;
};
}
}

#endif // POSTGRESQLFEATURECONNECTOR_H
