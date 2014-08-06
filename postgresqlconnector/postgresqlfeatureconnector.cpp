#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "coverage.h"
#include "feature.h"
#include "featurecoverage.h"

#include "postgresqlconnector.h"
#include "postgresqltableloader.h"
#include "postgresqldatabaseutil.h"
#include "postgresqlfeatureconnector.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlFeatureConnector::PostgresqlFeatureConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
    PostgresqlDatabaseUtil::openForResource(source(),"featureconnector");
}

PostgresqlFeatureConnector::~PostgresqlFeatureConnector()
{
    QSqlDatabase::removeDatabase("featureconnector");
}

IlwisObject *PostgresqlFeatureConnector::create() const
{
    qDebug() << "PostgresqlTableConnector::create() -> FeatureCoverage";
    return new FeatureCoverage(_resource);
}

ConnectorInterface *PostgresqlFeatureConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    qDebug() << "PostgresqlTableConnector::create() -> connector instance";
    return new PostgresqlFeatureConnector(resource, load,options);
}

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    qDebug() << "PostgresqlFeatureConnector::loadMetaData()";

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    fcoverage->setFeatureCount(itFEATURE, 0,0);
    fcoverage->featureTypes(itFEATURE);
    if ( !prepareTableForFeatureCoverage(fcoverage)) {
        ERROR1(ERR_NO_INITIALIZED_1, source().name());
        return false;
    }

    ITable table = fcoverage->attributeTable();
    if (sizeof(table->column(FEATUREIDCOLUMN)) != 0) {
        setFeatureCount(fcoverage);
        setSpatialMetadata(fcoverage);
    }

    // TODO further metadata?!

    return true;
}

void PostgresqlFeatureConnector::setFeatureCount(FeatureCoverage *fcoverage) const
{
    qDebug() << "PostgresqlFeatureConnector::setFeatureCount()";

    QString qTablename = PostgresqlDatabaseUtil::qTableFromTableResource(source());

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

void PostgresqlFeatureConnector::setSpatialMetadata(FeatureCoverage *fcoverage) const
{
    qDebug() << "PostgresqlFeatureConnector::setSpatialMetadata()";

    QStringList geometryColumns;
    QString qTablename = PostgresqlDatabaseUtil::qTableFromTableResource(source());
    PostgresqlDatabaseUtil::geometryColumnNames(source(), geometryColumns);

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
                QSqlRecord record = sridQuery.record();
                int sridIdx = record.indexOf("srid");
                int hasSridIdx = record.indexOf("isset");

                if (sridQuery.next()) {
                    if (sridQuery.value(hasSridIdx).toBool()) {
                        QString srid = sridQuery.value(sridIdx).toString();
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

bool PostgresqlFeatureConnector::prepareTableForFeatureCoverage(FeatureCoverage *fcoverage) const
{
    qDebug() << "PostgresqlFeatureConnector::prepareTableForFeatureCoverage()";

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

    PostgresqlTableLoader loader(source());
    if ( !loader.loadMetadata(featureTable.ptr())) {
        ERROR1("Could not load table metadata for table '%1'", featureTable->name());
        return false;
    }

    fcoverage->attributeTable(featureTable);
    return true;
}

bool PostgresqlFeatureConnector::loadData(IlwisObject *data)
{
    qDebug() << "PostgresqlFeatureConnector::loadData()";

    return false;
}

bool PostgresqlFeatureConnector::store(IlwisObject *data)
{
    qDebug() << "PostgresqlFeatureConnector::store()";

    return false;
}
