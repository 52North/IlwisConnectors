#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDir>

#include "kernel.h"
#include "raster.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "ellipsoid.h"
#include "conventionalcoordinatesystem.h"
#include "projection.h"
#include "numericrange.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "pixeliterator.h"
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

    IGeoReference grf;
    if(!grf.prepare(_resource.url().toLocalFile()))
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Georeference",gcoverage->name() );

    gcoverage->georeference(grf);

    Size sz(gdal()->xsize(_dataSet), gdal()->ysize(_dataSet), gdal()->layerCount(_dataSet));
    gcoverage->size(sz);

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

    return true;
}

inline double GridCoverageConnector::value(char *block, int index) const{
    double v = rUNDEF;
    char *c = &(block[index * _typeSize]);
    switch (_gdalValueType) {
    case GDT_Byte:
        v = *(qint8 *)c; break;
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
    GridCoverage *gc = static_cast<GridCoverage *>(data);
    Grid *grid = 0;
    if ( grid == 0) {

        Size sz = gc->size();
        grid =new Grid(sz);
    }
    grid->prepare();
    quint32 linesPerBlock = grid->maxLines();
    qint64 blockSizeBytes = grid->blockSize(0) * _typeSize;
    char *block = new char[blockSizeBytes];
    int count = 0; // block count over all the layers
    quint64 totalLines =grid->size().ysize();
    quint32 layer = 1;
    while(layer <= gc->size().zsize()) {
        quint64 linesLeft = totalLines;
        int gdalindex = 0; // count within one gdal layer
        while(true) {
            if ( block == 0) {
                kernel()->issues()->log(TR("Corrupt or invalid data size when reading data(GDAL connector)"));
                return 0;
            }
            if ( linesLeft > linesPerBlock)
                gdal()->rasterIO(layerHandle,GF_Read,0,gdalindex * linesPerBlock,grid->size().xsize(), linesPerBlock,
                                 block,grid->size().xsize(), linesPerBlock,_gdalValueType,0,0 );
            else {
                gdal()->rasterIO(layerHandle,GF_Read,0,gdalindex * linesPerBlock,grid->size().xsize(), linesLeft,
                                 block,grid->size().xsize(), linesLeft,_gdalValueType,0,0 );

            }
            quint32 noItems = grid->blockSize(count);
             if ( noItems == iUNDEF)
                return 0;
            std::vector<double> values(noItems);
            for(quint32 i=0; i < noItems; ++i) {
                double v = value(block, i);
                values[i] = v;
            }
            grid->setBlock(count, values, true);
            ++count;
            ++gdalindex;
            if ( linesLeft < linesPerBlock )
                break;
            linesLeft -= linesPerBlock;
        }
        if ( ++layer < gc->size().zsize())
            layerHandle = gdal()->getRasterBand(_dataSet, layer);
    }

    delete [] block;
    return grid;
}

Ilwis::IlwisObject *GridCoverageConnector::create() const{
    return new GridCoverage(_resource);
}

bool GridCoverageConnector::store(IlwisObject *obj, int )
{
    bool ok = GdalConnector::store(obj, 0);
    if ( !ok)
        return false;
    GridCoverage *gcov = static_cast<GridCoverage *>(obj);
    Size sz = gcov->size();
    GDALDataType gdalType = ilwisType2GdalType(gcov->datadef().range()->determineType());
    GDALDriverH hdriver = gdal()->getGDALDriverByName(_gdalShortName.toLocal8Bit());
    if ( hdriver == 0) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "driver",_gdalShortName);
    }
    const char *cext = gdal()->getMetaDataItem(hdriver,GDAL_DMD_EXTENSION,NULL);
    QString filename = _filename;
    if ( cext != 0 ) {
        QString ext(cext);
        int index = filename.lastIndexOf(".");
        if ( index != -1) {
            QString pext = filename.right(filename.size() - index);
            if ( pext.toLower() != ext) {
                filename += "." + ext;
            }
        }else
           filename += "." + ext;

    }

    GDALDatasetH dataset = gdal()->create( hdriver, filename.toLocal8Bit(), sz.xsize(), sz.ysize(), sz.zsize(), gdalType, 0 );
    if ( dataset == 0) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "data set",_filename);
    }
    if ( gcov->georeference()->grfType<CornersGeoReference>()) {
        //std::vector<double> mat = gcov->georeference()->impl<CornersGeoReference>()->matrix();
        std::vector<double> sup = gcov->georeference()->impl<CornersGeoReference>()->support();
        Box2Dd env = gcov->envelope();
        double a2 = (env.max_corner().x() - env.min_corner().x()) / gcov->size().xsize();
        double b2 = (env.max_corner().y() - env.min_corner().y()) / gcov->size().ysize();
        double geoTransform[6] = { env.min_corner().x(), a2, sup[0], env.min_corner().y(), sup[1], b2 };

        CPLErr err = gdal()->setGeoTransform(dataset,geoTransform);
        if ( err != CP_NONE) {
            kernel()->issues()->log(QString(gdal()->getLastErrorMsg()));
            gdal()->close(dataset);
            return false;
        }
        IConventionalCoordinateSystem csy = gcov->coordinateSystem().get<ConventionalCoordinateSystem>();
        QString proj4def = csy->projection()->toProj4();
        OGRSpatialReferenceH srsH = gdal()->newSRS(0);
        OGRErr errOgr = gdal()->importFromProj4(srsH, proj4def.toLocal8Bit());
        char *wktText = NULL;
        gdal()->exportToWkt(srsH,&wktText);
        err = gdal()->setProjection(dataset, wktText);
        gdal()->free(wktText);
        if ( err != CP_NONE) {
            kernel()->issues()->log(QString(gdal()->getLastErrorMsg()));
            gdal()->close(dataset);
            return false;
        }

    }

    switch(gdalType) {
    case GDT_Byte:
        ok = save<quint8>(gcov, dataset,gdalType);break;
    case GDT_UInt16:
        ok = save<quint16>(gcov, dataset,gdalType);break;
    case GDT_Int16:
        ok = save<qint16>(gcov, dataset,gdalType);break;
    case GDT_Int32:
        ok = save<qint32>(gcov, dataset,gdalType);break;
    case GDT_UInt32:
        ok = save<quint32>(gcov, dataset,gdalType);break;
    case GDT_Float32:
        ok = save<float>(gcov, dataset,gdalType);break;
    case GDT_Float64:
        ok = save<double>(gcov, dataset,gdalType);break;
    default:
        ok= ERROR1(ERR_NO_INITIALIZED_1, "gdal Data type");
    }

    gdal()->close(dataset);

    return ok;
}
