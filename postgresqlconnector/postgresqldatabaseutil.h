#ifndef POSTGRESQLDATABASEUTIL_H
#define POSTGRESQLDATABASEUTIL_H

#include <QSqlDatabase>
#include <QSqlQuery>

#include "kernel.h"
#include "resource.h"

namespace Ilwis {
namespace Postgresql {


class PostgresqlDatabaseUtil {

public:

    /**
     * @brief connectionFromResource creates a named database connection. Logs a
     * warning if expected user credentials are missing or empty.
     * @param resource the resource containing user credentials ('pg.user' and 'pg.password').
     * @param connectionname the name of the connection.
     * @return a database connection setup with the given user credentials.
     */
    static QSqlDatabase connectionFromResource(const Resource resource, QString connectionname="", bool open=true) {

        QUrl url = resource.url();
        if (connectionname.isEmpty()) {
            connectionname = resource.url().toString();
        }

        QSqlDatabase db = QSqlDatabase::database(connectionname);
        if (db.isValid()) {
            if (open && !db.isOpen()) {
                QString error = db.lastError().text();
                QString connection = resource.url(true).toString();
                ERROR2("Cannot establish connection to %1 (%2)", connection, error);
            }
            return db;
        }

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
     * @brief tablenameFromResource extracts the raw tablename (without schema qualifier) from
     * a given resource, by parsing the appropriate parts from its URL.
     * @param resource the resource where to extract the raw tablename from.
     * @return the unqualified table name
     */
    static QString tablenameFromResource(const Resource resource) {
        QStringList pathElements = resource.url().path().split("/", QString::SkipEmptyParts);
        return pathElements.at(2); // skip db and schema
    }

    /**
     * @brief qTableFromTableResource extracts the qualified tablename (with schema qualifier)
     * from a given resource, by parsing the appropriate parts from its URL.
     * @param resource the resource where to extract the qualified tablename from.
     * @return the qualified table name (e.g. schema.tablename)
     */
    static QString qTableFromTableResource(const Resource resource) {
        QStringList pathElements = resource.url().path().split("/", QString::SkipEmptyParts);
        QString schema(pathElements.at(1)); // skip db name
        return schema.append(".").append(pathElements.at(2));
    }

    static QString getInternalNameFrom(QString name, quint64 id) {
        return QString("ilwis://internalcatalog/%1_%2").arg(name).arg(id);
    }

    static void geometryColumnNames(const Resource resource, QStringList &columns) {
        QString tablename = PostgresqlDatabaseUtil::tablenameFromResource(resource);
        QString sqlBuilder;
        sqlBuilder.append("SELECT");
        sqlBuilder.append(" column_name ");
        sqlBuilder.append(" FROM ");
        sqlBuilder.append(" information_schema.columns ");
        sqlBuilder.append(" WHERE ");
        sqlBuilder.append(" table_name = '").append(tablename).append("' ");
        sqlBuilder.append(" AND ");
        sqlBuilder.append(" udt_name = 'geometry' ;");
        qDebug() << "SQL: " << sqlBuilder;

        QSqlDatabase db = PostgresqlDatabaseUtil::connectionFromResource(resource);
        QSqlQuery query = db.exec(sqlBuilder);

        while (query.next()) {
            columns.push_back(query.value(0).toString());
        }

        db.close();
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