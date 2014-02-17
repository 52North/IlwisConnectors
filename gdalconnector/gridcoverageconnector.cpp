#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDir>

#include "kernel.h"
#include "raster.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"

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
#include "geodeticdatum.h"
#include "projection.h"
#include "ellipsoid.h"
#include "conventionalcoordinatesystem.h"

using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *RasterCoverageConnector::create(const Resource &resource, bool load) {
    return new RasterCoverageConnector(resource, load);

}


RasterCoverageConnector::RasterCoverageConnector(const Ilwis::Resource &resource, bool load) : CoverageConnector(resource,load){
}

bool RasterCoverageConnector::loadMetaData(IlwisObject *data){

    if(!CoverageConnector::loadMetaData(data))
        return false;

    auto *gcoverage = static_cast<RasterCoverage *>(data);

    if (_handle->type() == GdalHandle::etGDALDatasetH){
        double geosys[6];
        CPLErr err = gdal()->getGeotransform(_handle->handle(), geosys) ;
        if ( err != CE_None) {
            return ERROR2(ERR_INVALID_PROPERTY_FOR_2, "Bounds", gcoverage->name());
        }

        double a1 = geosys[0];
        double b1 = geosys[3];
        double a2 = geosys[1];
        double b2 = geosys[5];
        Pixel pix(gdal()->xsize(_handle->handle()), gdal()->ysize(_handle->handle()));
        Coordinate crdLeftup( a1 , b1);
        Coordinate crdRightDown(a1 + pix.x * a2, b1 + pix.y * b2 ) ;
        Coordinate cMin( min(crdLeftup.x, crdRightDown.x), min(crdLeftup.y, crdRightDown.y));
        Coordinate cMax( max(crdLeftup.x, crdRightDown.x), max(crdLeftup.y, crdRightDown.y));

        gcoverage->envelope(Envelope(cMin, cMax));
        gcoverage->coordinateSystem()->envelope(gcoverage->envelope());

        IGeoReference grf;
        if(!grf.prepare(_resource.url().toLocalFile()))
            return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Georeference",gcoverage->name() );

        Size<> sz(gdal()->xsize(_handle->handle()), gdal()->ysize(_handle->handle()), gdal()->layerCount(_handle->handle()));
        grf->size(sz);
        gcoverage->georeference(grf);

        gcoverage->size(sz);

        int layerIndex = 1;
        auto index = _internalPath.indexOf("layerindex=");
        if ( index == 0) {
            bool ok = true;
            layerIndex = _internalPath.mid(11).toInt(&ok);
            if (!ok)
                return ERROR2(ERR_COULD_NOT_LOAD_2,gcoverage->name(),"layer");
        }
        auto layerHandle = gdal()->getRasterBand(_handle->handle(), layerIndex);
        if (!layerHandle) {
            return ERROR2(ERR_COULD_NOT_LOAD_2, gcoverage->name(),"layer");
        }

        int ok;

        auto vmin = gdal()->minValue(layerHandle, &ok);
        auto vmax = gdal()->maxValue(layerHandle, &ok);
        QString domName = NumericDomain::standardNumericDomainName(vmin, vmax);
        IDomain dom;
        dom.prepare(domName);
        if(!dom.isValid()) {
            return ERROR1(ERR_FIND_SYSTEM_OBJECT_1, domName);
        }
        INumericDomain numdom = dom.get<NumericDomain>();
        gcoverage->datadef().domain(dom);
        gcoverage->datadef().range(new NumericRange(vmin, vmax, numdom->range2range<NumericRange>()->resolution()));

        _gdalValueType = gdal()->rasterDataType(layerHandle);
        _typeSize = gdal()->getDataTypeSize(_gdalValueType) / 8;

        return true;
    }else{
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"non-RasterCoverage",_filename.toLocalFile());
    }
}

inline double RasterCoverageConnector::value(char *block, int index) const{
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
Grid *RasterCoverageConnector::loadGridData(IlwisObject* data){
    auto layerHandle = gdal()->getRasterBand(_handle->handle(), 1);
    if (!layerHandle) {
        ERROR2(ERR_COULD_NOT_LOAD_2, "GDAL","layer");
        return 0;
    }
    RasterCoverage *raster = static_cast<RasterCoverage *>(data);
    Grid *grid = 0;
    if ( grid == 0) {

        Size<> sz = raster->size();
        grid =new Grid(sz);
    }
    grid->prepare();
    quint32 linesPerBlock = grid->maxLines();
    qint64 blockSizeBytes = grid->blockSize(0) * _typeSize;
    char *block = new char[blockSizeBytes];
    int count = 0; // block count over all the layers
    quint64 totalLines =grid->size().ysize();
    quint32 layer = 1;
    while(layer <= raster->size().zsize()) {
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
        if ( ++layer < raster->size().zsize())
            layerHandle = gdal()->getRasterBand(_handle->handle(), layer);
    }

    delete [] block;
    _binaryIsLoaded = true;
    return grid;
}

Ilwis::IlwisObject *RasterCoverageConnector::create() const{
    return new RasterCoverage(_resource);
}

bool RasterCoverageConnector::setGeotransform(RasterCoverage *raster,GDALDatasetH dataset) {
    if ( raster->georeference()->grfType<CornersGeoReference>()) {
        std::vector<double> sup = raster->georeference()->impl<CornersGeoReference>()->support();
        Envelope env = raster->envelope();
        double a2 = (env.max_corner().x - env.min_corner().x) / raster->size().xsize();
        double b2 = (env.max_corner().y - env.min_corner().y) / raster->size().ysize();
        double geoTransform[6] = { env.min_corner().x, a2, sup[0], env.min_corner().y, sup[1], b2 };

        CPLErr err = gdal()->setGeoTransform(dataset,geoTransform);
        if ( err != CE_None) {
            reportError(dataset);
            return false;
        }
        return true;
    }
    return ERROR2(ERR_OPERATION_NOTSUPPORTED2,TR("Georeference type"), "Gdal");
}

bool RasterCoverageConnector::loadDriver()
{
    _driver = gdal()->getGDALDriverByName(_gdalShortName.toLocal8Bit());
    if ( !_driver ) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, "data-source", _filename.toString());
    }
    const char* metaitem = gdal()->getMetaDataItem(_driver, GDAL_DCAP_CREATE, NULL);
    if (QString(metaitem).toLower() != "yes") {
        return ERROR2(ERR_OPERATION_NOTSUPPORTED2, "write data-source", _filename.toString());
    }

    return true;
}

bool RasterCoverageConnector::store(IlwisObject *obj, int )
{
    if(!loadDriver())
        return false;

    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);

    IDomain dom;
    if(!dom.prepare("code=value")) { //TODO:  for the moment only value maps in gdal
        return ERROR1(ERR_NO_INITIALIZED_1,obj->name());
    }
    raster->datadef().domain(dom);
    Size<> sz = raster->size();
    GDALDataType gdalType = ilwisType2GdalType(raster->datadef().range()->determineType());
    QString filename = constructOutputName(_driver);

    GDALDatasetH dataset = gdal()->create( _driver, filename.toLocal8Bit(), sz.xsize(), sz.ysize(), sz.zsize(), gdalType, 0 );
    if ( dataset == 0) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "data set",_filename.toLocalFile());
    }
    bool ok = setGeotransform(raster, dataset);
    if (ok)
        ok = setSRS(raster, dataset);

    if (!ok)
        return false;

    switch(gdalType) {
    case GDT_Byte:
        ok = save<quint8>(raster, dataset,gdalType);break;
    case GDT_UInt16:
        ok = save<quint16>(raster, dataset,gdalType);break;
    case GDT_Int16:
        ok = save<qint16>(raster, dataset,gdalType);break;
    case GDT_Int32:
        ok = save<qint32>(raster, dataset,gdalType);break;
    case GDT_UInt32:
        ok = save<quint32>(raster, dataset,gdalType);break;
    case GDT_Float32:
        ok = save<float>(raster, dataset,gdalType);break;
    case GDT_Float64:
        ok = save<double>(raster, dataset,gdalType);break;
    default:
        ok= ERROR1(ERR_NO_INITIALIZED_1, "gdal Data type");
    }

    gdal()->close(dataset);

    return ok;
}

bool RasterCoverageConnector::setSRS(Coverage *raster, GDALDatasetH dataset) const
{
    OGRSpatialReferenceH srsH = createSRS(raster->coordinateSystem());
    if ( srsH == 0) {
        reportError(dataset);
        return false;
    }
    char *wktText = NULL;
    gdal()->exportToWkt(srsH,&wktText);
    CPLErr err = gdal()->setProjection(dataset, wktText);
    gdal()->free(wktText);
    if ( err != CE_None) {
        reportError(dataset);
        return false;
    }
    return true;
}


void RasterCoverageConnector::reportError(GDALDatasetH dataset) const
{
    kernel()->issues()->log(QString(gdal()->getLastErrorMsg()));
    gdal()->close(dataset);
}

