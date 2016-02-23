#include <iostream>
#include <array>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coverage.h"
#include "domain.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "range.h"
#include "identifierrange.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "rastercoverage.h"
#include "raster.h"
#include "pixeliterator.h"
#include "geometryhelper.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "postgresqlconnector.h"
#include "postgresqlrastercoverageloader.h"
#include "postgresqldatabaseutil.h"
#include "sqlstatementhelper.h"



using namespace Ilwis;
using namespace Postgresql;


PostgresqlRasterCoverageLoader::PostgresqlRasterCoverageLoader(const Resource &resource, const IOOptions &options) : _resource(resource), _options(options)
{

}

PostgresqlRasterCoverageLoader::~PostgresqlRasterCoverageLoader()
{
    QSqlDatabase::removeDatabase("rastercoverageloader");
}

bool PostgresqlRasterCoverageLoader::loadMetadata(RasterCoverage *coverage) const
{
    setSpatialMetadata(coverage);
    QList<MetaRasterColumn> metaGeometries;
    PostgresqlDatabaseUtil pgUtil(_resource,_options);
    pgUtil.getMetaForRasterColumns(metaGeometries);
    QList<QString> primaryKeys;
    pgUtil.getPrimaryKeys(primaryKeys);

    return true;
}

bool PostgresqlRasterCoverageLoader::loadData(RasterCoverage *coverage) const
{
    PostgresqlDatabaseUtil pgUtil(_resource,_options);
    QList<MetaRasterColumn> metaRasters;
    pgUtil.getMetaForRasterColumns(metaRasters);
    QString band = getBandPixelType(metaRasters);
    QSqlQuery bandQuery = pgUtil.doQuery(band, "rastercoverageloader");
    QString bandPixelType = "";
    if(bandQuery.next()){
        bandPixelType = bandQuery.value(0).toString();
    }
    int w = 0.0, h = 0.0;
    QSqlQuery addQuery = pgUtil.doQuery(getAdditionalInfo(metaRasters),"rastercoverageloader");
    if(addQuery.next()){
       w = addQuery.value(0).toDouble(0);
       h = addQuery.value(1).toDouble(0);
    }


    ICoordinateSystem crs;
    QString selectQuery = selectRasters(metaRasters);
    QSqlQuery query = pgUtil.doQuery(selectQuery, "rastercoverageloader");
    quint32 rows = metaRasters.size();
    IlwisTypes type;
    int headerSize = 61;
    int bandHeaderSize = 1;
    quint32 count = 0;

    IDomain dom = setDomain(bandPixelType);
    coverage->datadefRef().domain(dom);
    type = setStoreType(bandPixelType,coverage->datadef());
    coverage->setBandDefinition(type,coverage->datadef());

    int xsize = coverage->size().xsize();
    int ysize = coverage->size().ysize();

    int numx = xsize/w;
    int numy = ysize/h;



    int startX = 0;
    int endX = xsize;
    int startY = 0;
    int endY = ysize;
    Pixel pix;
    UPGrid& grid = coverage->gridRef();
    quint32 linesPerBlock = grid->maxLines();
    quint64 totalLines =grid->size().ysize();
    while (query.next()) {
         if(query.size() > 1){
            if(count == 0){
                startX = 0;
                endX = coverage->size().xsize()/numx;
                startY = 0;
                endY = coverage->size().ysize()/numy;
            }
            if(count == 1){
                startX = coverage->size().xsize()/numx;
                endX = coverage->size().xsize();
                startY = 0;
                endY == coverage->size().ysize()/numy;
            }
            if(count == 2){
                startX = 0;
                endX = coverage->size().xsize()/numx;
                startY = coverage->size().ysize()/numy;
                endY = coverage->size().ysize();
            }
            if(count == 3 ){
                startX = coverage->size().xsize()/numx;
                endX = coverage->size().xsize();
                startY = coverage->size().ysize()/numy;
                endY = coverage->size().ysize();
            }
        }

        if (rows > 0) {
            std::string result = query.value(0).toString().toStdString();
            int j = 0;
            switch(type){
            case itINT8:
                {
                    result = result.substr(124,result.size());
                    int size = 2;
                    qint64 blockSizeBytes = grid->blockSize(0) * size;
                    char *block = new char[blockSizeBytes];
                    int chunks = result.length() / size + ((result.length() % size > 0) ? 1 : 0);
                    std::vector<std::string> arr(chunks);
                    int i = 0;
                    int pixelsize= (endY-startY)*(endX-startX);
                    std::vector<double> values(pixelsize);

                    for(int m = startY; m < endY; m++){
                        pix.y = m;
                        for(int n = startX; n < endX; n++){
                            pix.x = n;
                            arr[j] = result.substr(i, size);
                            std::string str = getString(arr[j]);
                            int sizeofBits = str.size();
                            char *arrayofBit=new char[sizeofBits+1];
                            arrayofBit[sizeofBits]=0;
                            memcpy(arrayofBit,str.c_str(),sizeofBits);
                            char* buffer = arrayofBit;
                            unsigned long res = convtodecnum(buffer);
                            values[j] = res;
                            i +=size;
                            j++;
                        }
                    }
                    grid->setBlockData(count, values);



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
                    std::vector<double> values((endY-startY)*(endX-startX));
                    for(int m = startY; m < endY; m++){
                        pix.y = m;
                        for(int n = startX; n < endX; n++){
                            pix.x = n;
                            arr[j] = result.substr(i, size);
                            std::string str = getString(arr[j]);
                            int sizeofBits = str.size();
                            char *arrayofBit=new char[sizeofBits+1];
                            arrayofBit[sizeofBits]=0;
                            memcpy(arrayofBit,str.c_str(),sizeofBits);
                            char* buffer = arrayofBit;
                            unsigned long res = convtodecnum(buffer);
                            values[j] = res;
                            i +=size;
                            j++;
                        }
                    }

                    grid->setBlockData(count, values);
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
                    std::vector<double> values(arr.size());
                    for(int t = 0; t = arr.size(); t++){
                        arr[t] = result.substr(i, size);
                        std::string str = getString(arr[j]);
                        int sizeofBits = str.size();
                        char *arrayofBit=new char[sizeofBits+1];
                        arrayofBit[sizeofBits]=0;
                        memcpy(arrayofBit,str.c_str(),sizeofBits);
                        char* buffer = arrayofBit;
                        unsigned long res = convtodecnum(buffer);
                        values[t] = res;
                        i +=size;
                    }
                    /*for(int m = startY; m < endY; m++){
                        pix.y = m;
                        for(int n = startX; n < endX; n++){
                            pix.x = n;
                            arr[j] = result.substr(i, size);
                            std::string str = getString(arr[j]);
                            int sizeofBits = str.size();
                            char *arrayofBit=new char[sizeofBits+1];
                            arrayofBit[sizeofBits]=0;
                            memcpy(arrayofBit,str.c_str(),sizeofBits);
                            char* buffer = arrayofBit;
                            unsigned long res = convtodecnum(buffer);
                            values[j] = res;
                            i +=size;
                            j++;
                        }
                    }*/
                    if(count == 0){
                        grid->setBlockData(count, values);
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
                    std::vector<double> values((endY-startY)*(endX-startX));
                    for(int m = startY; m < endY; m++){
                        pix.y = m;
                        for(int n = startX; n < endX; n++){
                            pix.x = n;
                            arr[j] = result.substr(i, size);
                            std::string str = getString(arr[j]);
                            int sizeofBits = str.size();
                            char *arrayofBit=new char[sizeofBits+1];
                            arrayofBit[sizeofBits]=0;
                            memcpy(arrayofBit,str.c_str(),sizeofBits);
                            char* buffer = arrayofBit;
                            unsigned long res = convtodecnum(buffer);
                            values[j] = res;
                            i +=size;
                            j++;
                        }
                    }
                    grid->setBlockData(count, values);
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
                std::vector<double> values((endY-startY)*(endX-startX));
                for(int m = startY; m < endY; m++){
                    pix.y = m;
                    for(int n = startX; n < endX; n++){
                        pix.x = n;
                        arr[j] = result.substr(i, size);
                        std::string str = getString(arr[j]);
                        int sizeofBits = str.size();
                        char *arrayofBit=new char[sizeofBits+1];
                        arrayofBit[sizeofBits]=0;
                        memcpy(arrayofBit,str.c_str(),sizeofBits);
                        char* buffer = arrayofBit;
                        unsigned long res = convtodecnum(buffer);
                        values[j] = res;
                        i +=size;
                        j++;
                    }
                }
                grid->setBlockData(count, values);
                }
                break;
            default:
                break;

            }
        }
        count++;
    }

    return true;
}

IlwisTypes PostgresqlRasterCoverageLoader::setStoreType(QString pixel_type, DataDefinition inf) const
{
    IlwisTypes stPostgres;
    NumericRange *range = 0;
    if (pixel_type == "1BB") {
        range = new NumericRange(0,255);
        inf.range(range);

        stPostgres = itINT8;
    } else if (pixel_type == "2BUI") {
        range = new NumericRange(0,255);
        inf.range(range);
        stPostgres = itINT8;
    } else if (pixel_type == "4BUI") {
        range = new NumericRange(0,255);
        inf.range(range);
        stPostgres = itINT8;
    } else if (pixel_type == "8BUI") {
        range = new NumericRange(0,255);
        inf.range(range);
        stPostgres = itINT8;
    } else if (pixel_type == "8BSI") {
        range = new NumericRange(-128,127);
        inf.range(range);
        stPostgres = itINT8;
    } else if (pixel_type == "16BSI") {
        range = new NumericRange(-SHRT_MAX,-SHRT_MAX-2);
        inf.range(range);
        stPostgres = itINT16;
    } else if (pixel_type == "16BUI") {
        range = new NumericRange(0,65535-2);
        inf.range(range);
        stPostgres = itINT16;
    } else if (pixel_type == "32BSI") {
        range = new NumericRange(-LONG_MAX+2,LONG_MAX-2);
        inf.range(range);
        stPostgres = itINT32;
    } else if (pixel_type == "32BUI") {
        range = new NumericRange(0,LONG_MAX-2);
        inf.range(range);
        stPostgres = itINT32;
    } else if (pixel_type == "32BF") {
        range = new NumericRange(-1e100, 1e100, 0.0);
        inf.range(range);
        stPostgres = itFLOAT;
    } else if (pixel_type == "64BF") {
        range = new NumericRange(-1e100, 1e100, 0.0);
        inf.range(range);
        stPostgres = itDOUBLE;
    } else {
        range = new NumericRange(-1e100, 1e100, 0.0);
        inf.range(range);
         stPostgres = itDOUBLE;
    }
    return stPostgres;
}

IDomain PostgresqlRasterCoverageLoader::setDomain(QString pixel_type) const
{
    IDomain dom;
    if (pixel_type == "1BB") {
        dom.prepare("count");
    } else if (pixel_type == "2BUI") {
        dom.prepare("count");
    } else if (pixel_type == "4BUI") {
        dom.prepare("count");
    } else if (pixel_type == "8BUI") {
        dom.prepare("count");
    } else if (pixel_type == "8BSI") {
        dom.prepare("count");
    } else if (pixel_type == "16BSI") {
        dom.prepare("count");
    } else if (pixel_type == "16BUI") {
        dom.prepare("count");
    } else if (pixel_type == "32BSI") {
        dom.prepare("count");
    } else if (pixel_type == "32BUI") {
       dom.prepare("count");
    } else if (pixel_type == "32BF") {
        dom.prepare("value");
    } else if (pixel_type == "64BF") {
       dom.prepare("value");
    } else {
        dom.prepare("value");
    }
    return dom;

}

std::string PostgresqlRasterCoverageLoader::getString(std::string str) const
{
    std::string result ="";

    if(str.length() == 2){
        result = str;
    }
    else if(str.length() == 4){
        std::string s1 = str.substr(0,2);
        std::string s2 = str.substr(2,2);
        result = s2+s1;
    }
    else if(str.length() == 8){
        std::string s1 = str.substr(0,2);
        std::string s2 = str.substr(2,2);
        std::string s3 = str.substr(4,2);
        std::string s4 = str.substr(6,2);
        result = s4.append(s3).append(s2).append(s1);
    }
    else if(str.length() == 16){
        std::string s1 = str.substr(0,2);
        std::string s2 = str.substr(2,2);
        std::string s3 = str.substr(4,2);
        std::string s4 = str.substr(6,2);
        std::string s5 = str.substr(8,2);
        std::string s6 = str.substr(10,2);
        std::string s7 = str.substr(12,2);
        std::string s8 = str.substr(14,2);
        result = s8+s7+s6+s5+s4+s3+s2+s1;
    }
    return result;
}
unsigned long PostgresqlRasterCoverageLoader::convtodecnum(char *hex) const
{
    char *hexstr;
    int length = 0;
    const int base = 16;     // Base of Hexadecimal Number
    unsigned long decnum = 0;
    int i;

    // Now Find the length of Hexadecimal Number
    for (hexstr = hex; *hexstr != '\0'; hexstr++)
    {
        length++;
    }

    // Now Find Hexadecimal Number
    hexstr = hex;
    for (i = 0; *hexstr != '\0' && i < length; i++, hexstr++)
    {
        // Compare *hexstr with ASCII values
        if (*hexstr >= 48 && *hexstr <= 57)   // is *hexstr Between 0-9
        {
            decnum += (((int)(*hexstr)) - 48) * pow(base, length - i - 1);
        }
        else if ((*hexstr >= 65 && *hexstr <= 70))   // is *hexstr Between A-F
        {
            decnum += (((int)(*hexstr)) - 55) * pow(base, length - i - 1);
        }
        else if (*hexstr >= 97 && *hexstr <= 102)   // is *hexstr Between a-f
        {
            decnum += (((int)(*hexstr)) - 87) * pow(base, length - i - 1);
        }
        else
        {
            decnum = rUNDEF;

        }
    }
    return decnum;
}

void PostgresqlRasterCoverageLoader::setSpatialMetadata(RasterCoverage *coverage) const
{
    QList<MetaRasterColumn> metaGeometries;
    PostgresqlDatabaseUtil pgUtil(_resource, _options);
    pgUtil.getMetaForRasterColumns(metaGeometries);
    Envelope bbox;
    ICoordinateSystem crs;
    double xlength;
    double ylength;
    foreach (MetaRasterColumn meta, metaGeometries) {
        QString sqlBuilder;
        sqlBuilder.append("SELECT ");
        sqlBuilder.append("st_extent( ");
        sqlBuilder.append(meta.rasterColumn);
        sqlBuilder.append("::geometry), ");
        sqlBuilder.append("st_ScaleX( ");
        sqlBuilder.append(meta.rasterColumn);
        sqlBuilder.append("), ");
        sqlBuilder.append("st_ScaleY( ");
        sqlBuilder.append(meta.rasterColumn);
        sqlBuilder.append(")");
        sqlBuilder.append(" FROM ");
        sqlBuilder.append(meta.qtablename());
        sqlBuilder.append(" GROUP BY rid");
        sqlBuilder.append(";");
        QSqlQuery selectQuery = pgUtil.doQuery(sqlBuilder, "rastercoverageloader");

        while (selectQuery.next()) {

            // TODO check with Martin how to handle multiple
            // geometries for one entity
            QString envString = selectQuery.value(0).toString();
            if ( !envString.isEmpty()) {
                Envelope envelope(envString);
                bbox += envelope;
            }
            xlength = selectQuery.value(1).toDouble();
            ylength = selectQuery.value(2).toDouble();

        }

        qDebug()<<sqlBuilder;
       if ( !crs.isValid() && meta.crs.isValid()) {
            crs = meta.crs;
        }
    }
    QString corners = bbox.toString();

    QStringList listCorners = corners.split(" ");
    int width = abs(round((listCorners.at(2).toDouble() - listCorners.at(0).toDouble())/xlength));
    int height = abs(round((listCorners.at(3).toDouble() - listCorners.at(1).toDouble())/ylength));
    Size<> size(width,height, 1);

    QString grfs = QString("code=georef:type=corners,csy=%1,envelope=%2,gridsize=%3").arg(crs->id()).arg(bbox.toString()).arg(size.toString());

    IGeoReference georeference(grfs);

    coverage->georeference(georeference);
    coverage->gridRef()->prepare(0,size);
    coverage->coordinateSystem(crs);
    coverage->envelope(bbox);

}

QString PostgresqlRasterCoverageLoader::getBandPixelType(const QList<MetaRasterColumn> &metaGeometry) const{
     QString columns;
     std::for_each(metaGeometry.begin(), metaGeometry.end(), [&columns](MetaRasterColumn meta){
         columns.append(meta.rasterColumn).append(") ");
     });
     QString selectSQL = "SELECT st_bandpixeltype(";
     selectSQL.append(columns);
     selectSQL.append("FROM ");
     PostgresqlDatabaseUtil pgUtil(_resource,_options);
     selectSQL.append(pgUtil.qTableFromTableResource());
     selectSQL.append(" LIMIT 1 ");
     selectSQL.append("");
     return selectSQL;
}
QString PostgresqlRasterCoverageLoader::getAdditionalInfo(const QList<MetaRasterColumn> &metaGeometry) const{
     QString columns;
     std::for_each(metaGeometry.begin(), metaGeometry.end(), [&columns](MetaRasterColumn meta){
         columns.append(meta.rasterColumn).append(") ");
     });
     QString selectSQL = "SELECT st_width(";
     selectSQL.append(columns);
     selectSQL.append(",");
     selectSQL.append("st_height(");
     selectSQL.append(columns);
     selectSQL.append("FROM ");
     PostgresqlDatabaseUtil pgUtil(_resource,_options);
     selectSQL.append(pgUtil.qTableFromTableResource());
     selectSQL.append(" LIMIT 1 ");
     selectSQL.append("");
     return selectSQL;
}

QString PostgresqlRasterCoverageLoader::selectRasters(const QList<MetaRasterColumn> &metaGeometry) const
{
    QString columns;
    std::for_each(metaGeometry.begin(), metaGeometry.end(), [&columns](MetaRasterColumn meta) {
        columns.append("ST_MapAlgebraExpr(");
        //columns.append("ST_AsBinary(");
        //columns.append(meta.rasterColumn);
        //columns.append(", FALSE)");
        columns.append(meta.rasterColumn).append(",");
        columns.append("1,");
        QString band = "NULL";
        columns.append(band).append(",");
        columns.append("'[").append(meta.rasterColumn).append("]')");
    });
    columns = columns.left(columns.size());

    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(columns);
    sqlBuilder.append("FROM ");
    //QString selectSQL = QString("SELECT ST_AsBinary(rast) FROM %1.%2 WHERE rid=1").arg(schema,table);
    PostgresqlDatabaseUtil pgUtil(_resource,_options);
    sqlBuilder.append(pgUtil.qTableFromTableResource()).append(" ORDER BY rid ASC");

    return sqlBuilder;
}


