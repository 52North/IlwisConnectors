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
#include "rastercoverage.h"
#include "postgresqlconnector.h"
#include "postgresqldatabaseutil.h"
#include "postgresqlrasterconnector.h"
#include "postgresqlrastercoverageloader.h"


using namespace Ilwis;
using namespace Postgresql;


PostgresqlRasterConnector::PostgresqlRasterConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
}

PostgresqlRasterConnector::~PostgresqlRasterConnector()
{

}

IlwisObject *PostgresqlRasterConnector::create() const
{
   return new RasterCoverage(_resource);
}

ConnectorInterface *PostgresqlRasterConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new PostgresqlRasterConnector(resource, load, options);
}

bool PostgresqlRasterConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{

    RasterCoverage *coverage = static_cast<RasterCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlRasterCoverageLoader loader = PostgresqlRasterCoverageLoader(source(), iooptions);
    if ( !loader.loadMetadata(coverage)) {
        ERROR1(ERR_NO_INITIALIZED_1, source().name());
        return false;
    }
    return true;
}

bool PostgresqlRasterConnector::loadData(IlwisObject *data, const IOOptions &options)
{

    RasterCoverage *coverage = static_cast<RasterCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlRasterCoverageLoader loader = PostgresqlRasterCoverageLoader(source(), iooptions);
    /*UPGrid& grid = coverage->gridRef();
    quint32 linesPerBlock = grid->maxLines();
    qDebug()<<"Grid Count: "<<linesPerBlock;
    qint64 blockSizeBytes = grid->blockSize(0) * 4;
    qDebug()<<"Block size Bytes: "<<blockSizeBytes;
    char *block = new char[blockSizeBytes];
    quint64 totalLines =grid->size().ysize();
    qDebug()<<"Total lines: "<<totalLines;
    std::map<quint32, std::vector<quint32> > blocklimits = grid->calcBlockLimits(iooptions);
    //QSqlQuery query = pgUtil.doQuery(selectQuery, "rastercoverageloader");
    int count;
    for(const auto& layer : blocklimits){
        count++;
        for(const auto& index : layer.second) {
            quint32 fileBlock = index - layer.first * grid->blocksPerBand();


        }
    }
    std::cout<<count<<std::endl;*/
    bool ok = loader.loadData(coverage);
    _binaryIsLoaded = ok;
    return true;
}

bool PostgresqlRasterConnector::store(IlwisObject *data, const IOOptions &options)
{  
    RasterCoverage *rcoverage = static_cast<RasterCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlRasterCoverageLoader loader = PostgresqlRasterCoverageLoader(source(), iooptions);
    //bool ok = loader.storeData(rcoverage);
    return true;
}

QString PostgresqlRasterConnector::trimAndRemoveLastCharacter(const QString &string)
{
    return string.left(string.trimmed().length() - 1);
}
