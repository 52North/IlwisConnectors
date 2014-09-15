#include "raster.h"
#include "version.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "domain.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
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

bool RasterSerializerV1::store(IlwisObject *obj, const IOOptions &options)
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

    std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::IlwisVersion, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;
    domainStreamer->store( raster->stackDefinition().domain().ptr(), options);
    std::vector<QString> indexes = raster->stackDefinition().indexes();
    _stream << indexes.size();
    for(auto index : indexes)
        _stream << index;


    for(int index = 0; index < raster->size().zsize(); ++index)   {
        const DataDefinition& def = raster->datadef(index);
        storeDataDefintion(def,_stream, options);

    }
    std::unique_ptr<DataInterface> grfstreamer(factory->create(Version::IlwisVersion, itGEOREF,_stream));
    if ( !grfstreamer)
        return false;

    if(!grfstreamer->store(raster->georeference().ptr(), options))
        return false;
    return true;

}

bool RasterSerializerV1::storeData(IlwisObject *obj, const IOOptions &options )
{
    _stream << itRASTER;
    _stream << Version::IlwisVersion;
    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
    NumericStatistics& stats = raster->statistics(ContainerStatistics<double>::pBASIC);
    qint16 digits = stats.significantDigits();
    double scale = digits == 0 ? 0 : std::pow(10,-digits);
    RawConverter converter(stats[ContainerStatistics<double>::pMIN], stats[ContainerStatistics<double>::pMAX],scale);

    _stream << stats[ContainerStatistics<double>::pMIN] << stats[ContainerStatistics<double>::pMAX] << scale;
    BoundingBox box;
    if (options.contains("lines")) {
        QStringList parts = options["lines"].toString().split(" ");
        quint32 layer = parts[0].toUInt();
        quint32 minlines = parts[1].toUInt();
        quint32 maxlines = parts[2].toUInt();
        _stream <<  layer <<minlines << maxlines;
        box = BoundingBox(Pixel(0, minlines,layer), Pixel(raster->size().xsize(),maxlines,layer));

    }else {
        quint32 undef = iUNDEF;
        _stream << undef << undef << undef;
    }
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
    {
        PixelIterator iter(rcoverage, box);
        while(iter != iter.end()){
            if ( count % STREAMBLOCKSIZE && _streamconnector->needFlush()){
                _streamconnector->flush(false);

            }
            ++count;
            _stream << *iter;
            ++iter;
        }
        break;
    }
    }

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    std::unique_ptr<DataInterface> tblStreamer(factory->create(Version::IlwisVersion, itTABLE,_stream));
    if ( !tblStreamer)
        return false;

    if ( raster->hasAttributes()){
        tblStreamer->store(raster->attributeTable().ptr(), options);
    } else{
        _stream << itUNKNOWN;
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

    std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::IlwisVersion, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;
    IDomain dom(type);
    domainStreamer->loadMetaData( dom.ptr(), options);
    size_t nrOfBands;
    _stream >> nrOfBands;
    std::vector<QString> variants(nrOfBands);
    for(int i =0; i < nrOfBands; ++i){
        _stream >> variants[i];
    }
    raster->stackDefinitionRef().setSubDefinition(dom, variants);

    _stream >> type;
    _stream >> version;

    std::unique_ptr<DataInterface> grfstreamer(factory->create(version, itGEOREF,_stream));
    if ( !grfstreamer)
        return false;
    IGeoReference georeference (type);
    grfstreamer->loadMetaData(georeference.ptr(), options)    ;
    raster->georeference(georeference);



    return true;
}


bool RasterSerializerV1::loadData(IlwisObject *data, const IOOptions &options)
{
    RasterCoverage *raster = static_cast<RasterCoverage *>(data);

    int startBlock = 0;
    double mmin,  mmax, mscale;
    quint32 layer, minline, maxline;
    _stream >> mmin >> mmax >> mscale;
    _stream >> layer >> minline >> maxline;
    std::vector<quint32> blockList;
    if ( layer == iUNDEF || minline == iUNDEF || maxline == iUNDEF){
        for(int i=0; i < raster->grid()->blocks(); ++i)
            blockList.push_back(i);
    }else {
        startBlock = layer * raster->grid()->blocksPerBand();
        for(quint32 startline = minline, count=0; startline < maxline; startline += raster->grid()->maxLines(), ++count ){
            blockList.push_back(startBlock + count);
        }
    }

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
    default:
            for(int i = 0; i < blockList.size(); ++i) {
                quint64 noOfPixels = raster->grid()->blockSize(blockList[i]);
                std::vector<double> values(noOfPixels);
                for(int j = 0; j < noOfPixels; ++j){
                    _stream >> values[j];
                }
                raster->gridRef()->setBlockData(blockList[i], values, true);
            }
            break;
        }
    }
    quint64 type;
    QString version;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    _stream >> type;
    if ( type != itUNKNOWN){
        _stream >> version;

        std::unique_ptr<DataInterface> tableStreamer(factory->create(version, itTABLE,_stream));
        if ( !tableStreamer)
            return false;
        FlatTable *tbl = new FlatTable();

        tableStreamer->loadMetaData(tbl,options);
        raster->attributeTable(tbl);
    }
    return true;
}

VersionedSerializer *RasterSerializerV1::create(QDataStream &stream)
{
    return new RasterSerializerV1(stream);
}
