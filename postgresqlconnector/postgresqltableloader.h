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

    QString select(QString columns) const;
    bool createColumnDefinition(Table *table, const QSqlQuery &query, QList<QString> &primaryKeys) const;
};

}
}
#endif // POSTGRESQLTABLELOADER_H
