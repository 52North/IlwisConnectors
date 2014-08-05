#include <QSqlDatabase>
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
    return new FeatureCoverage(_resource);
}

ConnectorInterface *PostgresqlFeatureConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlFeatureConnector(resource, load,options);
}

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{

    qDebug() << "PostgresqlFeatureConnector::loadMetaData()";

    if ( !PostgresqlConnector::loadMetaData(data, options)) {
        return false;
    }

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
        setEnvelope(fcoverage);
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
    qDebug() << "SQL: " << sqlBuilder;

    QSqlDatabase db = QSqlDatabase::database("featureconnector");
    QSqlQuery query = db.exec(sqlBuilder);

    if (query.next()) {
        IlwisTypes types = itFEATURE;
        qint32 featureCount = query.value(0).toInt();
        fcoverage->setFeatureCount(types, featureCount, 0);
    }
}

void PostgresqlFeatureConnector::setEnvelope(FeatureCoverage *fcoverage) const
{
    qDebug() << "PostgresqlFeatureConnector::setEnvelope()";

    QStringList geometryColumns;
    QString qTablename = PostgresqlDatabaseUtil::qTableFromTableResource(source());
    PostgresqlDatabaseUtil::geometryColumnNames(source(), geometryColumns);

    if (geometryColumns.size() > 0) {
        QString sqlBuilder;
        sqlBuilder.append("SELECT ");
        // TODO adjust select for multiple geom columns
        sqlBuilder.append("st_extent( ");
        sqlBuilder.append(geometryColumns.at(0));
        sqlBuilder.append(" ) ");
        sqlBuilder.append(" FROM ");
        sqlBuilder.append(qTablename);
        sqlBuilder.append(";");
        qDebug() << "SQL: " << sqlBuilder;

        QSqlDatabase db = QSqlDatabase::database("featureconnector");
        QSqlQuery query = db.exec(sqlBuilder);

        if (query.next()) {
            Envelope bbox(query.value(0).toString());
            fcoverage->envelope(bbox);
        }
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
