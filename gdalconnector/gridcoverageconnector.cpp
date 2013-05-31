#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDir>

#include "kernel.h"
#include "raster.h"
#include "numericrange.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"
#include "coverageconnector.h"
#include "gridcoverageconnector.h"

using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *GridCoverageConnector::create(const Resource &item, bool load) {
    return new GridCoverageConnector(item, load);

}


GridCoverageConnector::GridCoverageConnector(const Ilwis::Resource &item, bool load) : CoverageConnector(item,load){
}

bool GridCoverageConnector::loadMetaData(IlwisObject *data){
    if(!CoverageConnector::loadMetaData(data))
        return false;

    auto *gcoverage = static_cast<GridCoverage *>(data);

    QSize sz(gdal()->xsize(_dataSet), gdal()->ysize(_dataSet));
    gcoverage->size(sz);


    IGeoReference grf;
    if(!grf.prepare(_resource.url().toLocalFile()))
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Georeference",gcoverage->name() );

    gcoverage->setGeoreference(grf);

    int layerIndex = 1;
    auto index = _internalPath.indexOf("layerindex=");
    if ( index == 0) {
        bool ok = true;
        layerIndex = _internalPath.mid(11).toInt(&ok);
        if (!ok)
            return ERROR2(ERR_COULD_NOT_LOAD_2,gcoverage->name(),"layer");
    }
    auto layerHandle = gdal()->getRasterBand(_dataSet, layerIndex);
    if (!layerHandle) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, gcoverage->name(),"layer");
    }

    int ok;

    auto vmin = gdal()->minValue(layerHandle, &ok);
    auto vmax = gdal()->maxValue(layerHandle, &ok);
    gcoverage->datadef().range(new NumericRange(vmin, vmax));

    _gdalValueType = gdal()->rasterDataType(layerHandle);
    _typeSize = gdal()->getDataTypeSize(_gdalValueType) / 8;

    _layers  = gdal()->layerCount(_dataSet);
    return true;
}

inline double GridCoverageConnector::value(char *block, int index) const{
    double v = rUNDEF;
    char *c = &(block[index * _typeSize]);
    switch (_gdalValueType) {
    case GDT_Byte:
        v = *(quint8 *)c; break;
    case GDT_Int16:
        v =  *(qint16 *)c; break;
    case GDT_UInt16:
        v =  *(quint16 *)c; break;
    case GDT_Int32:
        v = *(qint32 *)c; break;
    case GDT_UInt32:
        v = *(quint32 *)c; break;
    case GDT_Float32:
        v = *(float *)c; break;
    case GDT_Float64:
        v = *(double *)c; break;
    default:
        v = rUNDEF;
    }
    return v;
}
Grid *GridCoverageConnector::loadGridData(IlwisObject* data){
    auto layerHandle = gdal()->getRasterBand(_dataSet, 1);
    if (!layerHandle) {
        ERROR2(ERR_COULD_NOT_LOAD_2, "GDAL","layer");
        return 0;
    }
    Grid *grid = 0;
    if ( grid == 0) {
        GridCoverage *gc = static_cast<GridCoverage *>(data);
        Size sz = gc->size();
        grid =new Grid(sz);
    }
    grid->prepare();
    quint32 linesPerBlock = grid->maxLines();
    qint64 blockSizeBytes = grid->blockSize(0) * _typeSize;
    char *block = new char[blockSizeBytes];
    int count = 0;
    quint64 totalLines =grid->size().ysize();
    quint64 linesLeft = totalLines;
    while(true) {
        if ( block == 0) {
            kernel()->issues()->log(TR("Corrupt or invalid data size when reading data(GDAL connector)"));
            return 0;
        }
        if ( linesLeft > linesPerBlock)
            gdal()->rasterIO(layerHandle,GF_Read,0,count * linesPerBlock,grid->size().xsize(), linesPerBlock,
                             block,grid->size().xsize(), linesPerBlock,_gdalValueType,0,0 );
        else {
            gdal()->rasterIO(layerHandle,GF_Read,0,count * linesPerBlock,grid->size().xsize(), linesLeft,
                             block,grid->size().xsize(), linesLeft,_gdalValueType,0,0 );
            break;
        }
        quint32 noItems = grid->blockSize(count);
         if ( noItems == iUNDEF)
            return 0;
        std::vector<double> values(noItems);
        for(quint32 i=0; i < noItems; ++i) {
            double v = value(block, i);
            values[i] = v;
        }

        ++count;
        linesLeft -= linesPerBlock;
    }

    delete [] block;
    return grid;
}

Ilwis::IlwisObject *GridCoverageConnector::create() const{
    return new GridCoverage(_resource);
}
