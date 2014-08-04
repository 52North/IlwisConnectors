#ifndef POSTGRESQLTABLELOADER_H
#define POSTGRESQLTABLELOADER_H

namespace Ilwis {
namespace Postgresql {


class PostgresqlTableLoader
{
public:
    PostgresqlTableLoader();

    bool loadMetadata(Ilwis::Table *table, Resource resource) const;

private:
    bool createColumnDefinition(Table *table, QSqlQuery *query) const;
};

}
}
#endif // POSTGRESQLTABLELOADER_H
