#ifndef POSTGRESQLTABLELOADER_H
#define POSTGRESQLTABLELOADER_H

namespace Ilwis {
namespace Postgresql {


class PostgresqlTableLoader
{
public:
    PostgresqlTableLoader();

    bool loadMetadata(Table *table, Resource resource) const;

    bool loadData(Table *table, Resource resource);

private:
    bool createColumnDefinition(Table *table, QSqlQuery *query) const;
};

}
}
#endif // POSTGRESQLTABLELOADER_H
