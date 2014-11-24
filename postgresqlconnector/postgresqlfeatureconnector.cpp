#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "geometryhelper.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "domain.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "range.h"
#include "identifierrange.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "coverage.h"
#include "featurecoverage.h"

#include "postgresqlconnector.h"
#include "postgresqltableconnector.h"
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
    //qDebug() << "PostgresqlTableConnector::create() -> FeatureCoverage";
    return new FeatureCoverage(_resource);
}

ConnectorInterface *PostgresqlFeatureConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlFeatureConnector(resource, load, options);
}

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    //qDebug() << "PostgresqlFeatureConnector::loadMetaData()";

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    PostgresqlFeatureCoverageLoader loader(source());
    if ( !loader.loadMetadata(fcoverage, options)) {
        ERROR1(ERR_NO_INITIALIZED_1, source().name());
        return false;
    }

    return true;
}

bool PostgresqlFeatureConnector::loadData(IlwisObject *data, const IOOptions& options)
{
    //qDebug() << "PostgresqlFeatureConnector::loadData()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    PostgresqlFeatureCoverageLoader loader(source());
    bool ok = loader.loadData(fcoverage, options);
    _binaryIsLoaded = ok;
    return ok;
}

bool PostgresqlFeatureConnector::store(IlwisObject *data, const IOOptions& options)
{
    //qDebug() << "PostgresqlFeatureConnector::store()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    PostgresqlFeatureCoverageLoader loader(source());
    bool ok = loader.storeData(fcoverage, options);
    return ok;
}


QString PostgresqlFeatureConnector::trimAndRemoveLastCharacter(const QString &string) {
    return string.left(string.trimmed().length() - 1);
}
