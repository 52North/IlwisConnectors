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
#include "postgresqlfeaturecoverageloader.h"

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

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const IOOptions &)
{
    qDebug() << "PostgresqlFeatureConnector::loadMetaData()";

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    fcoverage->setFeatureCount(itFEATURE, 0,0);

    PostgresqlFeatureCoverageLoader loader(source());
    if ( !loader.loadMetadata(fcoverage)) {
        ERROR1(ERR_NO_INITIALIZED_1, source().name());
        return false;
    }

    return true;
}

bool PostgresqlFeatureConnector::loadData(IlwisObject *data, const IOOptions& options)
{
    qDebug() << "PostgresqlFeatureConnector::loadData()";

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
//    if ( !loadMetaData(fcoverage)) {
//        ERROR1(ERR_NO_INITIALIZED_1, source().name());
//        return false;
//    }
    PostgresqlFeatureCoverageLoader loader(source());
    return loader.loadData(fcoverage);
}

bool PostgresqlFeatureConnector::store(IlwisObject *data)
{
    qDebug() << "PostgresqlFeatureConnector::store()";

    // TODO store data back to table

    // store features

    return false;
}
