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
#include "featurecoverage.h"
#include "feature.h"

#include "postgresqlconnector.h"
#include "postgresqltableloader.h"
#include "postgresqldatabaseutil.h"
#include "postgresqlfeatureconnector.h"
#include "postgresqlfeaturecoverageloader.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlFeatureConnector::PostgresqlFeatureConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
}

PostgresqlFeatureConnector::~PostgresqlFeatureConnector()
{
}

IlwisObject *PostgresqlFeatureConnector::create() const
{
    qDebug() << "PostgresqlTableConnector::create() -> FeatureCoverage";
    return new FeatureCoverage(_resource);
}

ConnectorInterface *PostgresqlFeatureConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlFeatureConnector(resource, load,options);
}

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const IOOptions &)
{
    qDebug() << "PostgresqlFeatureConnector::loadMetaData()";

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
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
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
//    if ( !loadMetaData(fcoverage)) {
//        ERROR1(ERR_NO_INITIALIZED_1, source().name());
//        return false;
//    }
    PostgresqlFeatureCoverageLoader loader(source());
    bool ok = loader.loadData(fcoverage);
    _binaryIsLoaded = ok;
    return ok;
}

bool PostgresqlFeatureConnector::store(IlwisObject *data, const IOOptions& options)
{
    qDebug() << "PostgresqlFeatureConnector::store()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    // TODO store data back to table

    // store features

    return false;
}
