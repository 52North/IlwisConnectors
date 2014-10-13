#ifndef SQLSTATEMENTHELPER_H
#define SQLSTATEMENTHELPER_H

namespace Ilwis {
namespace Postgresql {

class SqlStatementHelper
{
public:
    SqlStatementHelper(const Resource &resource);
    ~SqlStatementHelper();

    void addCreateTempTableStmt(const QString &tmpTableName);
    void addInsertChangedDataToTempTableStmt(const QString &tmpTable, const Table *table);
    void addUpdateStmt(const QString &tmpTable, const Table *table);
    void addInsertStmt(const QString &tmpTable, const Table *table);

    QString sql();

private:
    Resource _resource;
    QList<QString> _tmpTables;
    QString _sqlBuilder;

    QString createWhereComparingPrimaryKeys(const QString &first, const QString &second) const;
    QString createInsertValueString(QVariant value, const ColumnDefinition &coldef) const;
    QString trimAndRemoveLastCharacter(const QString &string) const;
    QString columnNamesCommaSeparated(const Table *table) const;

};

typedef std::shared_ptr<SqlStatementHelper> SPSqlStatementHelper;

}
}

#endif // SQLSTATEMENTHELPER_H
