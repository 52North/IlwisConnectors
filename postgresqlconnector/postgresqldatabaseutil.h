#ifndef POSTGRESQLDATABASEUTIL_H
#define POSTGRESQLDATABASEUTIL_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>

#include "kernel.h"
#include "resource.h"
#include "geometries.h"
#include "coordinatesystem.h"

namespace Ilwis {

namespace Postgresql {

struct MetaGeometryColumn {
    QString catalog;
    QString schema = "public";
    QString tableName;
    QString geomColumn;
    int dimension;
    ICoordinateSystem crs;
    IlwisTypes geomType;
    QString qtablename() {
        QString qtablename = QString(schema);
        qtablename.append(".").append(tableName);
        return qtablename;
    }
};

class PostgresqlDatabaseUtil {

public:

    /**
     * @brief openForResource creates a named database connection.
     *
     * Creates a named database connection. The connection is being opened by default. Logs a warning
     * if expected user credentials are missing or empty. Database can be retrieved by
     * \code{.cpp}
     *  QSqlDatabase::database("connectionname");
     * \endcode
     * A connectionname can be empty, then the default connection is being used.
     * @param resource the resource containing user credentials ('pg.user' and 'pg.password').
     * @param connectionname the name of the connection.
     * @return a database connection setup with the given user credentials.
     */
    static QSqlDatabase openForResource(const Resource resource, QString connectionname="", bool open=true) {

        QUrl url = resource.url();
        if (connectionname.isEmpty()) {
            connectionname = resource.url().toString();
        }

        QSqlDatabase db;
        if (QSqlDatabase::contains(connectionname)) {
            db = QSqlDatabase::database(connectionname);
        } else {
            qDebug() << "add pg database connection" << connectionname;
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

            QString username = resource["pg.user"].toString();
            QString password = resource["pg.password"].toString();
            validateNotNullOrEmpty("Username", username);
            validateNotNullOrEmpty("Password", password);

            db.setUserName(username);
            db.setPassword(password);
        }

        if (open) {
            if ( !db.open()) {
                QString error = db.lastError().text();
                QString connection = resource.url(true).toString();
                ERROR2("Cannot establish connection to %1 (%2)", connection, error);
            }
        }

        return db;
    }

    /**
     * @brief copyResourceWithProperties copies a resource by also copying the database
     * properties needed to access the database (user credentials, schema).
     * @param resource the resource to copy
     * @return a copied instance
     */
    static Resource copyWithPropertiesAndType(const Resource resource, IlwisTypes newType) {
        Resource copy(resource.url(true), newType);
        copy.addProperty("pg.user", resource["pg.user"].toString());
        copy.addProperty("pg.password", resource["pg.password"].toString());
        copy.addProperty("pg.schema", resource["pg.schema"].toString());
        return copy;
    }

    /**
     * @brief tablenameFromResource extracts the raw tablename (without schema qualifier) from
     * a given resource, by parsing the appropriate parts from its URL.
     * @param resource the resource where to extract the raw tablename from.
     * @return the unqualified table name
     */
    static QString tablenameFromResource(const Resource resource) {
        QStringList pathElements = resource.url().path().split("/", QString::SkipEmptyParts);

        return pathElements.size() == 2
                ? pathElements.at(1) // schema omitted
                : pathElements.at(2); // skip db and schema
    }

    /**
     * @brief qTableFromTableResource extracts the qualified tablename (with schema qualifier)
     * from a given resource, by parsing the appropriate parts from its URL.
     * @param resource the resource where to extract the qualified tablename from.
     * @return the qualified table name (e.g. schema.tablename)
     */
    static QString qTableFromTableResource(const Resource resource) {
        QStringList pathElements = resource.url().path().split("/", QString::SkipEmptyParts);
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

    static QString getInternalNameFrom(QString name, quint64 id) {
        return QString("ilwis://internalcatalog/%1_%2").arg(name).arg(id);
    }

    static void getMetaForGeometryColumns(const Resource resource, QList<MetaGeometryColumn> &columns) {
        QString qtablename = qTableFromTableResource(resource);
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

        QSqlDatabase db = PostgresqlDatabaseUtil::openForResource(resource,"tmp");
        QSqlQuery query = db.exec(sqlBuilder);

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

        db.close(); // util class has to close after each select
    }

    static void prepareCoordinateSystem(QString srid, ICoordinateSystem &crs) {
        QString code = QString("code=epsg:%1").arg(srid);
        if ( !crs.prepare(code, itCONVENTIONALCOORDSYSTEM)) {
            ERROR1("Could not prepare crs with %1.", code);
        }
    }

private:
    static void validateNotNullOrEmpty(QString parameter, QVariant value) {
        if (value.isNull() || !value.isValid()) {
            WARN1("Property '%1' is null or empty.", parameter);
        }
    }
};

}
}
#endif // POSTGRESQLDATABASEUTIL_H
