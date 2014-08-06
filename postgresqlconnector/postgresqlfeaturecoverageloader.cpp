#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "feature.h"
#include "coverage.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"

#include "postgresqlfeaturecoverageloader.h"
#include "postgresqltableloader.h"
#include "postgresqldatabaseutil.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlFeatureCoverageLoader::PostgresqlFeatureCoverageLoader(const Resource resource): _resource(resource)
{
    PostgresqlDatabaseUtil::openForResource(_resource,"featurecoverageloader");
}

PostgresqlFeatureCoverageLoader::~PostgresqlFeatureCoverageLoader()
{
    QSqlDatabase::removeDatabase("featurecoverageloader");
}

bool PostgresqlFeatureCoverageLoader::loadMetadata(FeatureCoverage *fcoverage) const
{
    qDebug() << "PostgresqlFeatureCoverageLoader::loadMetadata()";

    QString name = fcoverage->name();
    quint64 id = fcoverage->id();
    QString schemaResource(PostgresqlDatabaseUtil::getInternalNameFrom(name, id));

    ITable featureTable;
    Resource resource(schemaResource, itFLATTABLE);
    if(!featureTable.prepare(resource)) {
        ERROR1(ERR_NO_INITIALIZED_1, resource.name());
        return false;
    }

    if ( !featureTable.isValid()) {
        ERROR0(TR("Could not prepare feature table for database feature."));
        return false;
    }

    PostgresqlTableLoader loader(_resource);
    if ( !loader.loadMetadata(featureTable.ptr())) {
        ERROR1("Could not load table metadata for table '%1'", featureTable->name());
        return false;
    }


    fcoverage->attributeTable(featureTable);
    if (sizeof(featureTable->column(FEATUREIDCOLUMN)) != 0) {
        setFeatureCount(fcoverage);
        setSpatialMetadata(fcoverage);
    }

    return true;
}

QSqlQuery PostgresqlFeatureCoverageLoader::selectGeometries(const QStringList geometryNames) const
{
    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(geometryNames.join(","));
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(PostgresqlDatabaseUtil::qTableFromTableResource(_resource));
    qDebug() << "SQL: " << sqlBuilder;

    QSqlDatabase db = QSqlDatabase::database("featurecoverageloader");
    return db.exec(sqlBuilder);
}

bool PostgresqlFeatureCoverageLoader::loadData(FeatureCoverage *fcoverage) const
{
    qDebug() << "PostgresqlFeatureCoverageLoader::loadData()";

    PostgresqlTableLoader loader(_resource);
    if ( !loader.loadData(fcoverage->attributeTable().ptr())) {

        // TODO handle exception message

    }

    QStringList geometryNames;
    PostgresqlDatabaseUtil::geometryColumnNames(_resource,geometryNames);
    QSqlQuery query = selectGeometries(geometryNames);

    while (query.next()) {
        foreach (QString colName, geometryNames) {
            QVariant variant = query.value(colName);

        }
    }


}

void PostgresqlFeatureCoverageLoader::setFeatureCount(FeatureCoverage *fcoverage) const
{
    qDebug() << "PostgresqlFeatureCoverageLoader::setFeatureCount()";

    QString qTablename = PostgresqlDatabaseUtil::qTableFromTableResource(_resource);

    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" count( ");
    sqlBuilder.append("*");
    sqlBuilder.append(" )");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(qTablename);
    sqlBuilder.append(";");
    //qDebug() << "SQL: " << sqlBuilder;

    QSqlDatabase db = QSqlDatabase::database("featureconnector");
    QSqlQuery query = db.exec(sqlBuilder);

    if (query.next()) {
        IlwisTypes types = itFEATURE;
        qint32 featureCount = query.value(0).toInt();
        fcoverage->setFeatureCount(types, featureCount, 0);
    }
}

void PostgresqlFeatureCoverageLoader::setSpatialMetadata(FeatureCoverage *fcoverage) const
{
    qDebug() << "PostgresqlFeatureCoverageLoader::setSpatialMetadata()";

    QStringList geometryColumns;
    QString qTablename = PostgresqlDatabaseUtil::qTableFromTableResource(_resource);
    PostgresqlDatabaseUtil::geometryColumnNames(_resource, geometryColumns);

    if (geometryColumns.size() > 0) {

        Envelope bbox;
        ICoordinateSystem crs;

        QSqlDatabase db = QSqlDatabase::database("featureconnector");
        foreach (QString geomColumn, geometryColumns) {
            QString sqlBuilder;
            sqlBuilder.append("SELECT ");
            sqlBuilder.append("st_extent( ").append(geomColumn).append(" ) ");
            sqlBuilder.append(" FROM ");
            sqlBuilder.append(qTablename);
            sqlBuilder.append(";");
            //qDebug() << "SQL: " << sqlBuilder;

            QSqlQuery envelopeQuery = db.exec(sqlBuilder);

            if (envelopeQuery.next()) {
                Envelope envelope(envelopeQuery.value(0).toString());
                bbox += envelope;
            }

            // first valid srid found is being considered as "main" crs.
            //
            // note that if multiple geom columns do exist, the geometries
            // have to be transformed to the "main" one when actual data is
            // loaded

            if ( !crs.isValid()) {
                sqlBuilder.clear();
                sqlBuilder.append("SELECT ");
                sqlBuilder.append(" st_srid( ").append(geomColumn).append(" ) AS srid, ");
                sqlBuilder.append(" st_srid( ").append(geomColumn).append(" ) is not null AS isset ");
                sqlBuilder.append(" FROM ");
                sqlBuilder.append(qTablename);
                sqlBuilder.append(" limit 1 ");
                sqlBuilder.append(";");
                //qDebug() << "SQL: " << sqlBuilder;

                QSqlQuery sridQuery = db.exec(sqlBuilder);

                if (sridQuery.next()) {
                    if (sridQuery.value("isset").toBool()) {
                        QString srid = sridQuery.value("srid").toString();
                        QString code = QString("code=epsg:%1").arg(srid);
                        if (crs.prepare(code, itCONVENTIONALCOORDSYSTEM)) {
                            fcoverage->coordinateSystem(crs);
                        } else {
                            ERROR1("Could not prepare crs with %1.", code);
                        }
                    }
                }
            }
        }
        fcoverage->envelope(bbox);
    }
}



