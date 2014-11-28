#ifndef POSTGRESQLDATABASEUTIL_H
#define POSTGRESQLDATABASEUTIL_H


#include "kernel.h"
#include "resource.h"
#include "geometries.h"
#include "coordinatesystem.h"

namespace Ilwis {

class SPFeatureI;

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

    PostgresqlDatabaseUtil(const Resource &resource, const IOOptions &options=IOOptions());
    ~PostgresqlDatabaseUtil();

    /**
     * @brief copyResourceWithProperties copies a resource by also copying the database
     * properties needed to access the database (user credentials, schema).
     * @return a copied instance
     */
    Resource resourceForType(IlwisTypes newType) const;

    /**
     * @brief tablenameFromResource extracts the raw tablename (without schema qualifier) from
     * a given resource, by parsing the appropriate parts from its URL.
     * @return the unqualified table name
     */
    QString tablenameFromResource() const;

    /**
     * @brief qTableFromTableResource extracts the qualified tablename (with schema qualifier)
     * from a given resource, by parsing the appropriate parts from its URL.
     * @return the qualified table name (e.g. schema.tablename)
     */
    QString qTableFromTableResource() const;

    QString getInternalNameFrom(QString name, quint64 id) const;

    void getMetaForGeometryColumns(QList<MetaGeometryColumn> &columns) const;

    bool exists(SPFeatureI feature) const;

    void getPrimaryKeys(QList<QString> &primaryColumns) const;

    void prepareCoordinateSystem(QString srid, ICoordinateSystem &crs) const;

    void prepareSubFeatureSemantics(IDomain &domain, const QList<MetaGeometryColumn> &geomColumns) const;

    QSqlQuery doQuery(QString stmt, QString connectionname="") const;

private:
    Resource _resource;
    IOOptions _options;

    /**
     * @brief openForResource creates a named database connection.
     *
     * Creates a named database connection. The connection is being opened by default. Logs a warning
     * if expected user credentials are missing or empty. Database can be retrieved by
     * \code{.cpp}
     *  QSqlDatabase::database("connectionname");
     * \endcode
     * A connectionname can be empty, then the default connection is being used.
     * @param connectionname the name of the connection.
     * @return a database connection setup with the given user credentials.
     */
    QSqlDatabase openForResource(QString connectionname="") const;

    void validateNotNullOrEmpty(QString parameter, QVariant value) const;

};

}
}
#endif // POSTGRESQLDATABASEUTIL_H
