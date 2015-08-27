#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>

#include "kernel.h"
#include "resource.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "domain.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "range.h"
#include "identifierrange.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "record.h"

#include "postgresqldatabaseutil.h"
#include "sqlstatementhelper.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlDatabaseUtil::PostgresqlDatabaseUtil(const Resource &resource, const IOOptions &options): _resource(resource), _options(options)
{
}

PostgresqlDatabaseUtil::~PostgresqlDatabaseUtil()
{
}

QSqlDatabase PostgresqlDatabaseUtil::openForResource(QString connectionname) const
{
    QUrl url = _resource.url();
    if (connectionname.isEmpty()) {
       connectionname = _resource.url().toString();
    }

    QSqlDatabase db;
    if (QSqlDatabase::contains(connectionname)) {
       db = QSqlDatabase::database(connectionname);
    } else {
       //qDebug() << "add pg database connection" << connectionname;
       db = QSqlDatabase::addDatabase("QPSQL", connectionname);

       qint64 port = url.port();
       QString host = url.host();
       QString path = url.path().split('/',QString::SkipEmptyParts).at(0);
       validateNotNullOrEmpty("Host", host);
       validateNotNullOrEmpty("Path", path);
       validateNotNullOrEmpty("Port", port);

       db.setHostName(host);
       db.setDatabaseName(path);
       db.setPort(port);

       QString username = _options.contains("pg.username")
               ? _options["pg.username"].toString()
               : "";
       QString password = _options.contains("pg.password")
               ? _options["pg.password"].toString()
               : "";
       validateNotNullOrEmpty("Username", username);
       validateNotNullOrEmpty("Password", password);

       db.setUserName(username);
       db.setPassword(password);
    }

    if ( !db.open()) {
       QString error = db.lastError().text();
       QString connection = _resource.url(true).toString();
       ERROR2("Cannot establish connection to %1 (%2)", connection, error);
    }
    return db;
}

Resource PostgresqlDatabaseUtil::resourceForType(IlwisTypes newType) const
{
    return Resource(_resource.url(true), newType);
}

QString Ilwis::Postgresql::PostgresqlDatabaseUtil::tablenameFromResource() const
{
   QStringList pathElements = _resource.url().path().split("/", QString::SkipEmptyParts);

   return pathElements.size() == 2
           ? pathElements.at(1) // schema omitted
           : pathElements.at(2); // skip db and schema
}

QString Ilwis::Postgresql::PostgresqlDatabaseUtil::qTableFromTableResource() const
{
    QStringList pathElements = _resource.url().path().split("/", QString::SkipEmptyParts);
    if (pathElements.size() == 2) {
       QString schema("public");
       QString tablename(pathElements.at(1)); // skip db name
       return  schema.append(".").append(tablename);
    } else {
       QString schema(pathElements.at(1)); // skip db name
       QString tablename = pathElements.at(2);
       return schema.append(".").append(tablename);
    }
}

QString Ilwis::Postgresql::PostgresqlDatabaseUtil::getInternalNameFrom(QString name, quint64 id) const
{
    return QString("ilwis://internalcatalog/%1_%2").arg(name).arg(id);
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::getMetaForGeometryColumns(QList<Ilwis::Postgresql::MetaGeometryColumn> &columns) const
{
    QString qtablename = qTableFromTableResource();
    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" * ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" geometry_columns ");
    if ( !qtablename.isEmpty()) {
        QStringList parts = qtablename.split(".");
        QString schema;
        QString table;
        if (parts.size() == 1) {
            schema = "public";
            table = parts.at(0);
        } else {
            schema = parts.at(0);
            table = parts.at(1);
        }
        sqlBuilder.append(" WHERE ");
        sqlBuilder.append(" f_table_schema = '").append(schema).append("' ");
        sqlBuilder.append(" AND ");
        sqlBuilder.append(" f_table_name = '").append(table).append("' ");
    }
    sqlBuilder.append(" ;");
    //qDebug() << "SQL: " << sqlBuilder;

    QSqlQuery query = doQuery(sqlBuilder, "tmp");

    while (query.next()) {
        MetaGeometryColumn meta;
        meta.catalog = query.value("f_table_catalog").toString();
        meta.schema = query.value("f_table_schema").toString();
        meta.tableName = query.value("f_table_name").toString();
        meta.geomColumn = query.value("f_geometry_column").toString();
        meta.dimension = query.value("coord_dimension").toInt();

        ICoordinateSystem crs;
        QString srid = QString::number(query.value("srid").toInt());
        prepareCoordinateSystem(srid, crs);
        meta.crs = crs;

        IlwisTypes ilwisType;
        QString geomType = query.value("type").toString();
        if (geomType.contains("LINE", Qt::CaseInsensitive) ) {
            ilwisType = itLINE;
        } else if (geomType.contains("POINT", Qt::CaseInsensitive) ) {
            ilwisType = itPOINT;
        } else if (geomType.contains("POLYGON", Qt::CaseInsensitive) ) {
            ilwisType = itPOLYGON;
        }
        meta.geomType = ilwisType;
        columns.push_back(meta);
    }

}

bool Ilwis::Postgresql::PostgresqlDatabaseUtil::exists(Ilwis::SPFeatureI feature) const
{
    PostgresqlDatabaseUtil pgUtil(_resource,_options);
    SqlStatementHelper sqlHelper(pgUtil);

    QString where;
    QList<QString> primaryKeys;
    getPrimaryKeys(primaryKeys);
    Record record = feature->record();
    foreach (QString primaryKey, primaryKeys) {
        if (where.isEmpty()) {
            where.append(" WHERE ");
        } else {
            where.append(" AND ");
        }
        ColumnDefinition coldef = feature->attributedefinition(primaryKey);
        QVariant value = record.cell(coldef.columnindex());

        where.append(primaryKey).append(" = ");
        where.append(sqlHelper.createInsertValueString(value, coldef));
    }

    QString sql = "SELECT ";
    //sql.append(" count(").append(primaryKeys.at(0)).append(") ");
    sql.append(QStringList(primaryKeys).join(","));
    sql.append(" FROM ").append(qTableFromTableResource());
    sql.append(where);

    QSqlQuery query = doQuery(sql, "exists");
    return query.next();
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::getPrimaryKeys(QList<QString> &primaryColumns) const
{
    QString qtablename = qTableFromTableResource();
    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" pg_attribute.attname ");
    //sqlBuilder.append(" , format_type(pg_attribute.atttypid, pg_attribute.atttypmod) ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" pg_index, pg_class, pg_attribute ");
    sqlBuilder.append(" WHERE ");
    sqlBuilder.append(" pg_class.oid = '").append(qtablename).append("'").append("::regclass ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append(" indrelid = pg_class.oid ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append("pg_attribute.attrelid = pg_class.oid ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append(" pg_attribute.attnum = any(pg_index.indkey) ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append(" indisprimary"); // ignore indexed only columns
    sqlBuilder.append(" ;");
    //qDebug() << "SQL: " << sqlBuilder;

    QSqlQuery query = doQuery(sqlBuilder, "tmp");

    while (query.next()) {
        primaryColumns.append(query.value(0).toString());
    }
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::prepareCoordinateSystem(QString srid, Ilwis::ICoordinateSystem &crs) const
{
    QString code = QString("code=epsg:%1").arg(srid);
    if ( !crs.prepare(code, itCONVENTIONALCOORDSYSTEM)) {
        ERROR1("Could not prepare crs with %1.", code);
    }
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::prepareSubFeatureSemantics(Ilwis::IDomain &domain, const QList<MetaGeometryColumn> &geomColumns) const
{
    QString columns;
    if ( !_options.contains("pg.features.order")) {
        auto join = [&columns](MetaGeometryColumn item) {
            QString column = item.geomColumn;
            return columns.isEmpty()
                    ? columns.append(column)
                    : columns.append(",").append(column);
        };
        std::for_each(geomColumns.begin(), geomColumns.end(), join);
    } else {
        columns = _options["pg.features.order"].toString();
    }
    QStringList orderedColumns = columns.split(",");
    NamedIdentifierRange priorities;
    foreach (QString column, orderedColumns) {
        priorities << column.trimmed();
    }
    IThematicDomain trackIdx;
    trackIdx.prepare();
    trackIdx->setRange(priorities);
    domain = trackIdx;
}

QSqlQuery Ilwis::Postgresql::PostgresqlDatabaseUtil::doQuery(QString stmt, QString connectionName) const
{
    QSqlDatabase db = openForResource(connectionName);
    QSqlQuery query(db);
    bool ok = query.exec(stmt);
    if ( !query.isActive()) {
        QString error = db.lastError().text();
        ERROR2("Could not execute sql statement (error: '%1'): '%2'", error, stmt);
    } else {
        //DEBUG1("SQL: '%1'", stmt);
    }
    return query;
}

void Ilwis::Postgresql::PostgresqlDatabaseUtil::validateNotNullOrEmpty(QString parameter, QVariant value) const
{
    if (value.isNull() || !value.isValid()) {
        WARN1("Property '%1' is null or empty.", parameter);
    }
}
