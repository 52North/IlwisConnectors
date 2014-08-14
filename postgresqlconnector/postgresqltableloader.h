#ifndef POSTGRESQLTABLELOADER_H
#define POSTGRESQLTABLELOADER_H

namespace Ilwis {

class Resource;
class FeatureCoverage;

namespace Postgresql {


class PostgresqlTableLoader
{
public:
    PostgresqlTableLoader(Resource resource);
    ~PostgresqlTableLoader();

    bool loadMetadata(Table *table) const;
    bool loadData(Table *table) const;

private:
    Resource _resource;

    QSqlQuery select(QString columns) const;
    bool createColumnDefinition(Table *table, QSqlQuery *query) const;
};

}
}
#endif // POSTGRESQLTABLELOADER_H
