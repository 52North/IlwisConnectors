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
#include "table.h"
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

/*bool PostgresqlRasterConnector::loadData(IlwisObject *data, const IOOptions &options)
{
    quint32 layer = source().hasProperty("bandindex") ? source()["bandindex"].toUInt(): iUNDEF;
    RasterCoverage *coverage = static_cast<RasterCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlRasterCoverageLoader loader = PostgresqlRasterCoverageLoader(source(), iooptions);
    bool ok = loader.loadData(coverage);
    _binaryIsLoaded = ok;
    return true;
}*/

bool PostgresqlRasterConnector::loadData(IlwisObject *data, const IOOptions &options)
{
    quint32 layer = source().hasProperty("bandindex") ? source()["bandindex"].toUInt(): iUNDEF;
    RasterCoverage *coverage = static_cast<RasterCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlRasterCoverageLoader loader = PostgresqlRasterCoverageLoader(source(), iooptions);
    PostgresqlDatabaseUtil pgUtil(_resource,iooptions);
    QList<MetaRasterColumn> metaRasters;
    pgUtil.getMetaForRasterColumns(metaRasters);
    QString band = loader.getBandPixelType(metaRasters);

    QSqlQuery bandQuery = pgUtil.doQuery(band, "rastercoverageloader");
    QString bandPixelType = "";
    if(bandQuery.next()){
        bandPixelType = bandQuery.value(0).toString();
    }

    int w = 0.0, h = 0.0;
    QSqlQuery addQuery = pgUtil.doQuery(loader.getAdditionalInfo(metaRasters),"rastercoverageloader");
    if(addQuery.next()){
       w = addQuery.value(0).toDouble(0);
       h = addQuery.value(1).toDouble(0);
    }
    ICoordinateSystem crs;
    QString selectQuery = loader.selectRasters(metaRasters);
    QSqlQuery query = pgUtil.doQuery(selectQuery, "rastercoverageloader");
    quint32 rows = metaRasters.size();
    IlwisTypes type;
    int headerSize = 61;
    int bandHeaderSize = 1;

    IDomain dom = loader.setDomain(bandPixelType);
    coverage->datadefRef().domain(dom);
    type = loader.setStoreType(bandPixelType,coverage->datadef());
    coverage->setBandDefinition(type,coverage->datadef());

    int xsize = coverage->size().xsize();
    int ysize = coverage->size().ysize();

    UPGrid& grid = coverage->gridRef();

    int pixSize = xsize*ysize;
    std::vector<double> values(pixSize);
    int x = 0;
    while(query.next()){
        std::string result = query.value(0).toString().toStdString();
        switch(type){
            case itINT8:
            {
                result = result.substr(124,result.size());
                int size = 2;
                int chunks = result.length() / size + ((result.length() % size > 0) ? 1 : 0);
                std::vector<std::string> arr(chunks);
                int i = 0;
                for(int j = 0; j < arr.size(); j++){
                    arr[j] = result.substr(i, size);
                    std::string str = loader.getString(arr[j]);
                    int sizeofBits = str.size();
                    char *arrayofBit=new char[sizeofBits+1];
                    arrayofBit[sizeofBits]=0;
                    memcpy(arrayofBit,str.c_str(),sizeofBits);
                    char* buffer = arrayofBit;
                    unsigned long res = loader.convtodecnum(buffer);
                    values[x] = res;
                    x++;
                    i +=size;
                }

            }
                break;
            case itINT16:
            {
                result = result.substr(128,result.size());
                int size = 4;
                qint64 blockSizeBytes = grid->blockSize(0) * size;
                char *block = new char[blockSizeBytes];
                int chunks = result.length() / size + ((result.length() % size > 0) ? 1 : 0);
                std::vector<std::string> arr(chunks);
                int i = 0;
                for(int j = 0; j < arr.size(); j++){
                    arr[j] = result.substr(i, size);
                    std::string str = loader.getString(arr[j]);
                    int sizeofBits = str.size();
                    char *arrayofBit=new char[sizeofBits+1];
                    arrayofBit[sizeofBits]=0;
                    memcpy(arrayofBit,str.c_str(),sizeofBits);
                    char* buffer = arrayofBit;
                    unsigned long res = loader.convtodecnum(buffer);
                    values[x] = res;
                    x++;
                    i +=size;
                }
            }
                break;
            case itINT32:
            {
                result = result.substr(132,result.size());
                int size = 8;
                qint64 blockSizeBytes = grid->blockSize(0) * size;
                char *block = new char[blockSizeBytes];
                int chunks = result.length() / size + ((result.length() % size > 0) ? 1 : 0);
                std::vector<std::string> arr(chunks);
                int i = 0;
                for(int j = 0; j < arr.size(); j++){
                    arr[j] = result.substr(i, size);
                    std::string str = loader.getString(arr[j]);
                    int sizeofBits = str.size();
                    char *arrayofBit=new char[sizeofBits+1];
                    arrayofBit[sizeofBits]=0;
                    memcpy(arrayofBit,str.c_str(),sizeofBits);
                    char* buffer = arrayofBit;
                    unsigned long res = loader.convtodecnum(buffer);
                    values[x] = res;
                    x++;
                    i +=size;
                }

            }
              break;
            case itFLOAT:
            {
                result = result.substr(132,result.size());
                int size = 8;
                qint64 blockSizeBytes = grid->blockSize(0) * size;
                char *block = new char[blockSizeBytes];
                int chunks = result.length() / size + ((result.length() % size > 0) ? 1 : 0);
                std::vector<std::string> arr(chunks);
                int i = 0;
                for(int j = 0; j < arr.size(); j++){
                    arr[j] = result.substr(i, size);
                    std::string str = loader.getString(arr[j]);
                    int sizeofBits = str.size();
                    char *arrayofBit=new char[sizeofBits+1];
                    arrayofBit[sizeofBits]=0;
                    memcpy(arrayofBit,str.c_str(),sizeofBits);
                    char* buffer = arrayofBit;
                    unsigned long res = loader.convtodecnum(buffer);
                    values[x] = res;
                    x++;
                    i +=size;
                }
            }
             break;
            case itDOUBLE:
            {
                result = result.substr(140,result.size());
                int size = 16;
                qint64 blockSizeBytes = grid->blockSize(0) * size;
                char *block = new char[blockSizeBytes];
                int chunks = result.length() / size + ((result.length() % size > 0) ? 1 : 0);
                std::vector<std::string> arr(chunks);
                int i = 0;
                for(int j = 0; j < arr.size(); j++){
                    arr[j] = result.substr(i, size);
                    std::string str = loader.getString(arr[j]);
                    int sizeofBits = str.size();
                    char *arrayofBit=new char[sizeofBits+1];
                    arrayofBit[sizeofBits]=0;
                    memcpy(arrayofBit,str.c_str(),sizeofBits);
                    char* buffer = arrayofBit;
                    unsigned long res = loader.convtodecnum(buffer);
                    values[x] = res;
                    x++;
                    i +=size;
                }
            }
              break;
            default:
              break;
        }
    }
    quint32 begin = 0;
    quint32 end = 0;
    for (int i = 0; i < grid->blocks(); i++){
        quint32 noItems = grid->blockSize(i);
        end = begin + noItems;
        std::vector<double> value(values.begin()+ begin, values.begin()+ end);
        grid->setBlockData(i, value);
        begin += noItems;
    }


    if ( coverage->attributeTable().isValid()) {
        ITable tbl = coverage->attributeTable();
        IDomain covdom;
        if (!covdom.prepare("count")){
            return false;
        }
        for(quint32 i=0; i < tbl->recordCount() ; ++i) {
            tbl->setCell(COVERAGEKEYCOLUMN,i, QVariant(i));
        }
    }
    _binaryIsLoaded = true;
    //bool ok = loader.loadData(coverage);
    //_binaryIsLoaded = ok;
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
