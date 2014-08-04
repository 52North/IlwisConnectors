
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDatabase>

#include "kernel.h"
#include "ilwisdata.h"
#include "ilwistypes.h"
#include "resource.h"
#include "domain.h"
#include "numericdomain.h"
#include "textdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"

#include "postgresqltableloader.h"
#include "postgresqldatabaseutil.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlTableLoader::PostgresqlTableLoader()
{
}

bool PostgresqlTableLoader::loadMetadata(Table *table, Resource resource) const
{
    qDebug() << "PostgresqlTableLoader::loadMetadata()";

    QString rawTablename(PostgresqlDatabaseUtil::tablenameFromResource(resource));
    QSqlDatabase db = PostgresqlDatabaseUtil::connectionFromResource(resource,"loadTableMetadata");
    if ( !db.isOpen()) {
        return false;
    }

    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" column_name,udt_name ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" information_schema.columns ");
    sqlBuilder.append(" WHERE ");
    sqlBuilder.append(" table_name='").append(rawTablename).append("';");
    qDebug() << "SQL: " << sqlBuilder;
    QSqlQuery query = db.exec(sqlBuilder);

    while (query.next()) {
        QString columnName = query.value(0).toString();
        if ( !createColumnDefinition(table, &query)) {
            if ( !query.isValid()) {
                WARN("no data record selected.");
            } else {
                DEBUG2("Ignore column '%1' in table '%2'", columnName, rawTablename);
            }
        }
    }

    db.close();
    return table->isValid();
}

bool PostgresqlTableLoader::loadData(Table *table, Resource resource)
{
    QSqlDatabase db = PostgresqlDatabaseUtil::connectionFromResource(resource, "loadData");
    if ( !db.isOpen()) {
        return false;
    }

    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" * ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(PostgresqlDatabaseUtil::qTableFromTableResource(resource));
    qDebug() << "SQL: " << sqlBuilder;
    QSqlQuery query = db.exec(sqlBuilder);

    quint64 count = 0;

    QSqlRecord recordDef = query.record();
    while (query.next()) {
        std::vector<QVariant> record(table->columnCount());
        for (int i = 0; i < table->columnCount(); i++) {
            ColumnDefinition& coldef = table->columndefinitionRef(i);


            if ( coldef.name() == QString(FEATUREIDCOLUMN) ) {
                continue; // auto filled column
            }

            DataDefinition& datadef = coldef.datadef();
            if( !datadef.domain().isValid()) {
                WARN2(ERR_NO_INITIALIZED_2, "domain", coldef.name());
                record[i] = QVariant(); // empty
                continue;
            }

            qint64 fieldIdx = recordDef.indexOf(coldef.name());
            record[i] = query.value(fieldIdx);

        }
        table->record(count++, record);
    }

    return true;
}

bool PostgresqlTableLoader::createColumnDefinition(Table *table, QSqlQuery *query) const
{
    if ( !query->isValid()) {
        WARN("Can't create columndefinition for an invalid data record.");
        return false;
    }

    QString columnName = query->value(0).toString();
    QString udtName = query->value(1).toString();

    IDomain domain;
    if (udtName == "varchar" || udtName == "text" || udtName == "char") {
        domain.prepare("code=domain:text",itTEXTDOMAIN);
    } else if (udtName == "timestamptz") {
        ITimeDomain tdomain;
        tdomain.prepare();
        tdomain->range(new TimeInterval(itDATE));
        domain = tdomain;
    } else if (udtName.startsWith("float")) {
        INumericDomain ndomain;
        ndomain.prepare("value");
        domain = domain;
    } else if (udtName.startsWith("int")) {
        INumericDomain ndomain;
        ndomain.prepare("integer");
        domain = domain;
    } else if (udtName == "bool") {
        domain.prepare("boolean", itBOOL);
    } else if (udtName == "geometry") {
        // table describes features
        table->addColumn(FEATUREIDCOLUMN, "count");
        return true;
    } else {
        MESSAGE1("No domain handle for db type '%1')", udtName);
        return false;
    }

    table->addColumn(columnName, domain);
    return true;
}
