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
    QString sqlBuilder;
    sqlBuilder.append(" CREATE TEMP TABLE ");
    sqlBuilder.append(tmpTableName);
    sqlBuilder.append(" ON COMMIT DROP AS ");
    sqlBuilder.append(" (SELECT * FROM ").append(templateTable).append(" )");
    sqlBuilder.append(" WITH NO DATA ; ");
    _sqlBuilder.append(sqlBuilder);
}

void SqlStatementHelper::addInsertChangedDataToTempTableStmt(const QString &tmpTable, const Table *table)
{
    // FILL TMP TABLE WITH UPDATE/INSERT DATA

    if ( !_tmpTables.contains(tmpTable)) {
        ERROR1("Temporary table '%1' must be created before inserting data.", tmpTable);
        return;
    }

    QString sqlBuilder;
    sqlBuilder.append(" INSERT INTO ").append(tmpTable).append(" ( ");
    sqlBuilder.append(columnNamesCommaSeparated(table));
    sqlBuilder.append(" ) VALUES ");
    for (int i = 0 ; i < table->recordCount() ; i++) {
        Record record = table->record(i);
        if (record.isChanged()) {

            // create value tuples
            sqlBuilder.append(" ( ");
            for (int j = 0 ; j < record.columnCount() ; j++) {
                ColumnDefinition coldef = table->columndefinition(j);
                QVariant value = record.cell(j);
                sqlBuilder.append(createInsertValueString(value, coldef));
                sqlBuilder.append(", ");
            }
            sqlBuilder = trimAndRemoveLastCharacter(sqlBuilder);
            sqlBuilder.append(" ), ");
        }
    }
    sqlBuilder = trimAndRemoveLastCharacter(sqlBuilder);
    sqlBuilder.append(" ; ");
    _sqlBuilder.append(sqlBuilder);
}

void SqlStatementHelper::addUpdateStmt(const QString &tmpTable, const Table *table)
{

    if ( !_tmpTables.contains(tmpTable)) {
        ERROR1("No data table '%1' present.", tmpTable);
        return;
    }

    QString sqlBuilder;
    QString qtablename = PostgresqlDatabaseUtil::qTableFromTableResource(_resource);
    sqlBuilder.append(" UPDATE ");
    sqlBuilder.append(qtablename);
    sqlBuilder.append(" AS current");
    sqlBuilder.append(" SET ");

    for (int i = 0 ; i < table->columnCount() ; i++) {
        ColumnDefinition coldef = table->columndefinition(i);
        if ( !coldef.isReadOnly()) {
            sqlBuilder.append(coldef.name());
            sqlBuilder.append(" = updated.").append(coldef.name());
            sqlBuilder.append(", ");
        }
    }
    QString columns = columnNamesCommaSeparated(table);
    sqlBuilder = trimAndRemoveLastCharacter(sqlBuilder);
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" ( SELECT ");
    sqlBuilder.append(columns);
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(tmpTable);
    sqlBuilder.append(" ) ");
    sqlBuilder.append(" AS updated ( ");
    sqlBuilder.append(columns);
    sqlBuilder.append(" ) ");

    sqlBuilder.append(createWhereComparingPrimaryKeys("updated", "current"));
    sqlBuilder.append(" ; ");
    _sqlBuilder.append(sqlBuilder);
}

void SqlStatementHelper::addInsertStmt(const QString &tmpTable, const Table *table)
{
    if ( !_tmpTables.contains(tmpTable)) {
        ERROR1("No data table '%1' present.", tmpTable);
        return;
    }

    QString sqlBuilder;
    QString qtablename = PostgresqlDatabaseUtil::qTableFromTableResource(_resource);
    sqlBuilder.append(" INSERT INTO ");
    sqlBuilder.append(qtablename);
    QString columns = columnNamesCommaSeparated(table);
    sqlBuilder.append(" ( ").append(columns).append(" ) ");
    sqlBuilder.append(" SELECT ").append(columns).append(" FROM ");
    sqlBuilder.append(tmpTable).append(" AS new ");
    sqlBuilder.append(" WHERE NOT EXISTS ( SELECT NULL FROM ");
    sqlBuilder.append(qtablename).append(" AS existing");
    sqlBuilder.append(createWhereComparingPrimaryKeys("new", "existing"));
    sqlBuilder.append(" ) ;");
    _sqlBuilder.append(sqlBuilder);
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
        whereClause.append(first).append(".").append(primaryKey);
        whereClause.append(" = ");
        whereClause.append(second).append(".").append(primaryKey);
        whereClause.append(" ");
    }
    return whereClause;
}

QString SqlStatementHelper::trimAndRemoveLastCharacter(const QString &string) const
{
    QString trimmed = string.trimmed();
    return trimmed.left(trimmed.length() - 1);
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


