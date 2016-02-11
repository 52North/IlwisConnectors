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
    return new FeatureCoverage(_resource);
}

ConnectorInterface *PostgresqlFeatureConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlFeatureConnector(resource, load, options);
}

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlFeatureCoverageLoader loader = PostgresqlFeatureCoverageLoader(source(), iooptions);
    if ( !loader.loadMetadata(fcoverage)) {
        ERROR1(ERR_NO_INITIALIZED_1, source().name());
        return false;
    }

    return true;
}

bool PostgresqlFeatureConnector::loadData(IlwisObject *data, const IOOptions& options)
{
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlFeatureCoverageLoader loader = PostgresqlFeatureCoverageLoader(source(), iooptions);
    bool ok = loader.loadData(fcoverage);
    _binaryIsLoaded = ok;
    return ok;
}



bool PostgresqlFeatureConnector::store(IlwisObject *data, const IOOptions& options)
{
    //qDebug() << "PostgresqlFeatureConnector::store()";
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlFeatureCoverageLoader loader = PostgresqlFeatureCoverageLoader(source(), iooptions);
    bool ok = loader.storeData(fcoverage);
    return ok;
}


QString PostgresqlFeatureConnector::trimAndRemoveLastCharacter(const QString &string) {
    return string.left(string.trimmed().length() - 1);
}
