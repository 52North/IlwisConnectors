
#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "domain.h"
#include "juliantime.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"

#include "postgresqldatabaseutil.h"
#include "postgresqlconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqltableloader.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlTableConnector::PostgresqlTableConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
}

IlwisObject *PostgresqlTableConnector::create() const
{
    qDebug() << "PostgresqlTableConnector::create() -> FlatTable";
    return new FlatTable(_resource);
}

ConnectorInterface *PostgresqlTableConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlTableConnector(resource, load, options);
}

bool PostgresqlTableConnector::loadMetaData(IlwisObject *data, const IOOptions& options)
{
    qDebug() << "PostgresqlTableConnector::loadMetaData()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    Table *table = static_cast<Table *>(data);
    PostgresqlTableLoader loader(source());
    return loader.loadMetadata(table);
}

bool PostgresqlTableConnector::store(IlwisObject *data, const IOOptions& options)
{
    qDebug() << "PostgresqlTableConnector::store()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    Table *table = static_cast<Table *>(data);

    QString sqlBuilder;
    sqlBuilder.append("UPDATE ");
    sqlBuilder.append(PostgresqlDatabaseUtil::qTableFromTableResource(_resource));
    sqlBuilder.append(" AS current");
    sqlBuilder.append(" SET ");

    for (int i = 0 ; i < table->columnCount() ; i++) {
        ColumnDefinition coldef = table->columndefinition(i);
        if ( !coldef.isReadOnly()) {
            sqlBuilder.append(coldef.name());
            sqlBuilder.append(" = updates.").append(coldef.name());
            sqlBuilder.append(", ");
        }
    }
    sqlBuilder = trimAndRemoveLastCharacter(sqlBuilder);
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" ( VALUES ");

    for (int i = 0 ; i < table->recordCount() ; i++) {
        Record record = table->record(i);
        if (record.isChanged()) {
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
    sqlBuilder.append(" ) AS updates ( ");

    for (int i = 0 ; i < table->columnCount() ; i++) {
        ColumnDefinition coldef = table->columndefinition(i);
        sqlBuilder.append(coldef.name());
        sqlBuilder.append(", ");
    }
    sqlBuilder = trimAndRemoveLastCharacter(sqlBuilder);
    sqlBuilder.append(" ) ");

    QString whereClause;

    for (int i = 0 ; i < table->columnCount(); i ++) {
        ColumnDefinition coldef = table->columndefinition(i);
        if (coldef.isReadOnly()) {
            if (whereClause.isEmpty()) {
                whereClause.append(" WHERE ");
            } else {
                whereClause.append(" AND ");
            }
            whereClause.append(" current.").append(coldef.name());
            whereClause.append(" = ");
            whereClause.append(" updates.").append(coldef.name());
        }
    }
    sqlBuilder.append(whereClause).append(" ;");
    qDebug() << "SQL: " << sqlBuilder;


    QSqlDatabase db = PostgresqlDatabaseUtil::openForResource(_resource,"updatetable");
    db.exec(sqlBuilder);
    db.close();

    return true;
}

QString PostgresqlTableConnector::trimAndRemoveLastCharacter(const QString &string) {
    return string.left(string.trimmed().length() - 1);
}

QString PostgresqlTableConnector::createInsertValueString(QVariant value, const ColumnDefinition &coldef) const {
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

bool PostgresqlTableConnector::loadData(IlwisObject *data,const IOOptions& options)
{
    qDebug() << "PostgresqlTableConnector::loadData()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    Table *table = static_cast<Table *>(data);
    PostgresqlTableLoader loader(source());
    if ( !loader.loadMetadata(table)) {
        return false;
    }
    bool ok = loader.loadData(table);
    _binaryIsLoaded = ok;
    return ok;
}

