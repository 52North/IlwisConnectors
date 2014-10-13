#include <QList>
#include <QVariant>

#include "kernel.h"
#include "ilwisdata.h"
#include "ilwistypes.h"
#include "domain.h"
#include "juliantime.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"

#include "postgresqldatabaseutil.h"
#include "sqlstatementhelper.h"

using namespace Ilwis;
using namespace Postgresql;

SqlStatementHelper::SqlStatementHelper(const Resource &resource): _resource(resource), _tmpTables( {0} )
{
}

SqlStatementHelper::~SqlStatementHelper()
{
}

void SqlStatementHelper::addCreateTempTableStmt(const QString &tmpTableName)
{
    // PREPARE TMP TABLE FOR UPDATE/INSERT DATA

    if (_tmpTables.contains(tmpTableName)) {
        ERROR1("Temporary table '%1' already exists.", tmpTableName);
        return;
    }

    _tmpTables.push_back(tmpTableName);
    QString templateTable = PostgresqlDatabaseUtil::qTableFromTableResource(_resource);
    _sqlBuilder.append(" CREATE TEMP TABLE ");
    _sqlBuilder.append(tmpTableName);
    _sqlBuilder.append(" ON COMMIT DROP AS ");
    _sqlBuilder.append(" (SELECT * FROM ").append(templateTable).append(" )");
    _sqlBuilder.append(" WITH NO DATA ; ");
}

void SqlStatementHelper::addInsertChangedDataToTempTableStmt(const QString &tmpTable, const Table *table)
{
    // FILL TMP TABLE WITH UPDATE/INSERT DATA

    if ( !_tmpTables.contains(tmpTable)) {
        ERROR1("Temporary table '%1' must be created before inserting data.", tmpTable);
        return;
    }

    _sqlBuilder.append(" INSERT INTO ").append(tmpTable).append(" ( ");
    _sqlBuilder.append(columnNamesCommaSeparated(table));
    _sqlBuilder.append(" ) VALUES ");
    for (int i = 0 ; i < table->recordCount() ; i++) {
        Record record = table->record(i);
        if (record.isChanged()) {

            // create value tuples
            _sqlBuilder.append(" ( ");
            for (int j = 0 ; j < record.columnCount() ; j++) {
                ColumnDefinition coldef = table->columndefinition(j);
                QVariant value = record.cell(j);
                _sqlBuilder.append(createInsertValueString(value, coldef));
                _sqlBuilder.append(", ");
            }
            _sqlBuilder = trimAndRemoveLastCharacter(_sqlBuilder);
            _sqlBuilder.append(" ), ");
        }
    }
    _sqlBuilder = trimAndRemoveLastCharacter(_sqlBuilder);
    _sqlBuilder.append(" ; ");
}

void SqlStatementHelper::addUpdateStmt(const QString &tmpTable, const Table *table)
{

    if ( !_tmpTables.contains(tmpTable)) {
        ERROR1("No data table '%1' present.", tmpTable);
        return;
    }

    QString qtablename = PostgresqlDatabaseUtil::qTableFromTableResource(_resource);
    _sqlBuilder.append(" UPDATE ");
    _sqlBuilder.append(qtablename);
    _sqlBuilder.append(" AS current");
    _sqlBuilder.append(" SET ");

    for (int i = 0 ; i < table->columnCount() ; i++) {
        ColumnDefinition coldef = table->columndefinition(i);
        if ( !coldef.isReadOnly()) {
            _sqlBuilder.append(coldef.name());
            _sqlBuilder.append(" = updated.").append(coldef.name());
            _sqlBuilder.append(", ");
        }
    }
    QString columns = columnNamesCommaSeparated(table);
    _sqlBuilder = trimAndRemoveLastCharacter(_sqlBuilder);
    _sqlBuilder.append(" FROM ");
    _sqlBuilder.append(" ( SELECT ( ");
    _sqlBuilder.append(columns);
    _sqlBuilder.append(" ) FROM ");
    _sqlBuilder.append(tmpTable);
    _sqlBuilder.append(" ) ");
    _sqlBuilder.append(" AS updated ( ");
    _sqlBuilder.append(columns);
    _sqlBuilder.append(" ) ");

    _sqlBuilder.append(createWhereComparingPrimaryKeys("updated", "current"));
    _sqlBuilder.append(" ; ");
}

void SqlStatementHelper::addInsertStmt(const QString &tmpTable, const Table *table)
{
    if ( !_tmpTables.contains(tmpTable)) {
        ERROR1("No data table '%1' present.", tmpTable);
        return;
    }

    QString qtablename = PostgresqlDatabaseUtil::qTableFromTableResource(_resource);
    _sqlBuilder.append(" INSERT INTO ");
    _sqlBuilder.append(qtablename);
    QString columns = columnNamesCommaSeparated(table);
    _sqlBuilder.append(" ( ").append(columns).append(" ) ");
    _sqlBuilder.append(" SELECT ").append(columns).append(" FROM ");
    _sqlBuilder.append(tmpTable).append(" AS new ");
    _sqlBuilder.append(" WHERE NOT EXISTS ( SELECT NULL FROM ");
    _sqlBuilder.append(qtablename).append(" AS existing");
    _sqlBuilder.append(createWhereComparingPrimaryKeys("new", "existing"));
    _sqlBuilder.append(" ) ;");
}

QString SqlStatementHelper::sql()
{
    return _sqlBuilder;
}

// private methods

QString SqlStatementHelper::createWhereComparingPrimaryKeys(const QString &first, const QString &second) const
{
    QString whereClause;
    QList<QString> primaryKeys;
    PostgresqlDatabaseUtil::getPrimaryKeys(_resource,primaryKeys);
    for (QString primaryKey : primaryKeys) {
        if (whereClause.isEmpty()) {
            whereClause.append(" WHERE ");
        } else {
            whereClause.append(" AND ");
        }
        whereClause.append(" .").append(first).append(primaryKey);
        whereClause.append(" = ");
        whereClause.append(" .").append(second).append(primaryKey);
    }
    return whereClause;
}

QString SqlStatementHelper::trimAndRemoveLastCharacter(const QString &string) const
{
    return string.left(string.trimmed().length() - 1);
}

QString SqlStatementHelper::columnNamesCommaSeparated(const Table *table) const
{
    QString columns;
    for (int i = 0 ; i < table->columnCount() ; i++) {
        ColumnDefinition coldef = table->columndefinition(i);
        columns.append(coldef.name());
        columns.append(", ");
    }
    return trimAndRemoveLastCharacter(columns);
}

QString SqlStatementHelper::createInsertValueString(QVariant value, const ColumnDefinition &coldef) const {
    IDomain domain = coldef.datadef().domain<>();
    if (hasType(domain->valueType(),itINTEGER)) {

        return QString::number(value.toInt());

    } else if (hasType(domain->valueType(),itDOUBLE | itFLOAT)) {

        return QString::number(value.toDouble());

    } else if (hasType(domain->valueType(),itTHEMATICITEM | itNAMEDITEM | itINDEXEDITEM | itNUMERICITEM | itTIMEITEM)) {

        return domain->impliedValue(value).toString();

    } else if (hasType(domain->valueType(), itDATETIME)) {
        if ( QString(value.typeName()).compare("Ilwis::Time") != 0){
            ERROR2(ERR_COULD_NOT_CONVERT_2,value.toString(), "time");
            Time time(0,0,0,0,0);
            return time.toString(itDATE);
        } else{
            Time time = value.value<Ilwis::Time>();
            return time.toString(itDATETIME);
        }
    } else if (hasType(domain->valueType(),itSTRING)){

        return QString("'%1'").arg(value.toString());

    } else {
        ERROR0("Could not determine data type.");
        return QString("");
    }
}


