#ifndef POSTGRESQLRASTERCONNECTOR_H
#define POSTGRESQLRASTERCONNECTOR_H

namespace Ilwis {

class RasterCoverage;

namespace Postgresql {

class PostgresqlRasterConnector : public PostgresqlConnector
{
public:
    PostgresqlRasterConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
    ~PostgresqlRasterConnector();


    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions& options=IOOptions());
    bool loadData(IlwisObject *data, const IOOptions &options=IOOptions());
    bool store(IlwisObject* data, const IOOptions &options);

private:
    QString trimAndRemoveLastCharacter(const QString &string);
};
}
}
#endif // POSTGRESQLRASTERCONNECTOR_H
