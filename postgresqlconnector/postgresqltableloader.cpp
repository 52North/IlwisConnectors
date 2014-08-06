
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

PostgresqlTableLoader::PostgresqlTableLoader(Resource resource): _resource(resource)
{
    PostgresqlDatabaseUtil::openForResource(_resource,"tableloader");
}

PostgresqlTableLoader::~PostgresqlTableLoader()
{
    QSqlDatabase::removeDatabase("tableloader");
}

bool PostgresqlTableLoader::loadMetadata(Table *table) const
{
    qDebug() << "PostgresqlTableLoader::loadMetadata()";

    QString rawTablename(PostgresqlDatabaseUtil::tablenameFromResource(_resource));

    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" column_name,udt_name ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" information_schema.columns ");
    sqlBuilder.append(" WHERE ");
    sqlBuilder.append(" table_name='").append(rawTablename).append("';");
    qDebug() << "SQL: " << sqlBuilder;

    QSqlDatabase db = QSqlDatabase::database("tableloader");
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

    return table->isValid();
}

QSqlQuery PostgresqlTableLoader::selectAll() const
{
    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" * ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(PostgresqlDatabaseUtil::qTableFromTableResource(_resource));
    qDebug() << "SQL: " << sqlBuilder;

    QSqlDatabase db = QSqlDatabase::database("tableloader");
    return db.exec(sqlBuilder);
}

bool PostgresqlTableLoader::loadTableData(Table *table) const
{
    QSqlQuery query = selectAll();
    QSqlRecord recordDef = query.record();

    quint64 count = 0;
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

            QStringList geometryNames;
            PostgresqlDatabaseUtil::geometryColumnNames(_resource,geometryNames);

            if (geometryNames.contains(coldef.name())) {
                continue; // TODO geometry column is loaded differently
            }

            qint64 fieldIdx = recordDef.indexOf(coldef.name());
            record[i] = query.value(fieldIdx);

        }
        table->record(count++, record);
    }

    return true;
}

bool PostgresqlTableLoader::loadFeatureCoverageData(FeatureCoverage *fcoverage) const
{
    return false;
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
        domain = ndomain;
    } else if (udtName.startsWith("int")) {
        INumericDomain ndomain;
        ndomain.prepare("integer");
        domain = ndomain;
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
