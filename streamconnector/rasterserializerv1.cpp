#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "coverage.h"
#include "raster.h"
#include "pixeliterator.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"
#include "ilwisobjectconnector.h"
#include "streamconnector.h"
#include "coverageserializerv1.h"
#include "rawconverter.h"
#include "rasterserializerv1.h"

using namespace Ilwis;
using namespace Stream;

RasterSerializerV1::RasterSerializerV1(QDataStream& stream) : CoverageSerializerV1(stream)
{
}

template<typename T> void storeBulk(const RawConverter& converter, QDataStream& stream, const IRasterCoverage& raster){
    for(double v : raster){
       stream << (T)converter.real2raw(v);
    }
}

template<typename T> void loadBulk(const RawConverter& converter, QDataStream& stream, const IRasterCoverage& raster){
    T value;
    for(double& v : raster){
       stream >> value;
       v = converter.raw2real(value);
    }
}

bool RasterSerializerV1::store(IlwisObject *obj, int options)
{
    if (!CoverageSerializerV1::store(obj, options))
        return false;
    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);


    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    _stream << raster->size().xsize() << raster->size().ysize() << raster->size().zsize();

    if(!storeDataDefintion(raster->datadef(), _stream, options))
        return false;
    for(int index = 0; index < raster->size().zsize(); ++index)   {
        const DataDefinition& def = raster->datadef(index);
        storeDataDefintion(def,_stream, options);
    }
    std::unique_ptr<DataInterface> grfstreamer(factory->create(Version::IlwisVersion, itGEOREF,_stream));
    if ( !grfstreamer)
        return false;

    if(!grfstreamer->store(raster->georeference().ptr(), options))
        return false;

    NumericStatistics& stats = raster->statistics(ContainerStatistics<double>::pBASIC);
    qint16 digits = stats.significantDigits();
    double scale = digits == 0 ? 0 : std::pow(10,-digits);
    RawConverter converter(stats[ContainerStatistics<double>::pMIN], stats[ContainerStatistics<double>::pMAX],scale);
    _stream << stats[ContainerStatistics<double>::pMIN] << stats[ContainerStatistics<double>::pMAX] << scale;
    quint64 count = 0;
    IRasterCoverage rcoverage(raster);
    switch (converter.storeType()){
    case itUINT8:
        storeBulk<quint8>(converter, _stream, rcoverage); break;
    case itINT16:
        storeBulk<qint16>(converter, _stream, rcoverage); break;
    case itINT32:
        storeBulk<qint32>(converter, _stream, rcoverage); break;
    case itINT64:
        for(double v : rcoverage)
            _stream << (qint64)v;
        break;
    default:
        for(double v : rcoverage){
            if ( count % STREAMBLOCKSIZE && _streamconnector->needFlush()){
                _streamconnector->flush(false);

            }
            ++count;
            _stream << v;
        }
        break;
    }
    return true;

}

bool RasterSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!CoverageSerializerV1::loadMetaData(obj, options))
        return false;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);


    quint32 xsize, ysize, zsize;
    _stream >> xsize >> ysize >> zsize;
    raster->size(Size<>(xsize, ysize, zsize));


    loadDataDefinition(raster->datadefRef(),_stream, options);
    for(int band = 0; band < raster->size().zsize(); ++band) {
        loadDataDefinition(raster->datadefRef(band), _stream, options)    ;
    }
    quint64 type;
    QString version;
    _stream >> type;
    _stream >> version;

    std::unique_ptr<DataInterface> grfstreamer(factory->create(version, itGEOREF,_stream));
    if ( !grfstreamer)
        return false;
    GeoReference *georeference = new GeoReference();
    grfstreamer->loadMetaData(georeference, options)    ;
    raster->georeference(georeference);



    return true;
}

bool RasterSerializerV1::loadData(IlwisObject *data, const IOOptions &options)
{
    RasterCoverage *raster = static_cast<RasterCoverage *>(data);

    double mmin,  mmax, mscale;
    _stream >> mmin >> mmax >> mscale;

    RawConverter converter(mmin, mmax, mscale);

    IRasterCoverage rcoverage(raster);
    switch (converter.storeType()){
    case itUINT8:
        loadBulk<quint8>(converter, _stream, rcoverage); break;
    case itINT16:
        loadBulk<qint16>(converter, _stream, rcoverage); break;
    case itINT32:
        loadBulk<qint32>(converter, _stream, rcoverage); break;
    case itINT64:{
        qint64 value;
        for(double &v : rcoverage){
            _stream >> value;
            v = value;
        }

        break;
    }
    default:
        for(double v : rcoverage)
            _stream >> v;
        break;
    }
    return true;
}

VersionedSerializer *RasterSerializerV1::create(QDataStream &stream)
{
    return new RasterSerializerV1(stream);
}
