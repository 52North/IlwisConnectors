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

template<typename T> void storeBulk(const RawConverter& converter, QDataStream& stream, StreamConnector *streamconnector, const BoundingBox& box, const IRasterCoverage& raster){
    quint64 count = streamconnector->position();
    if ( streamconnector->isFileBased()){
        const UPGrid& grid = raster->grid();
        quint32 blockCount = grid->blocks();
        stream << blockCount;
        for(quint32 i = 0; i < blockCount; ++i){
            const char * data = grid->blockAsMemory(i);
            quint64 blockSize = 8 * grid->blockSize(i);
            stream << i;
            stream << blockSize;
            stream.writeRawData(data, blockSize);
        }
        streamconnector->flush(true);
    }else {
        PixelIterator iter(raster, box);
        while(iter != iter.end()){
            if ( count >= STREAMBLOCKSIZE - 9 ) {
                streamconnector->flush(false);
                count = 0;

            }
            count += sizeof(T);
            stream << (T)converter.real2raw(*iter);
            ++iter;
        }
    }
}

template<typename T> void loadBulk(const RawConverter& converter, QDataStream& stream, StreamConnector *streamconnector, const BoundingBox& box, const IRasterCoverage& raster){

    if ( streamconnector->isFileBased()){
        //const UPGrid& grid = raster->grid();
        quint32 blockCount;
        stream >> blockCount;
        for(int i = 0; i < blockCount; ++i){
            quint32 blockIndex;
            quint64 blockByteSize;

            stream >> blockIndex;
            stream >> blockByteSize;
            int pixelCount = blockByteSize / 8;
            std::vector<double> data(pixelCount);
            stream.readRawData((char *)&data[0],blockByteSize );
            raster->gridRef()->setBlockData(i, data);
        }
    streamconnector->flush(true);
    } else {
     // TODO network restore, roughly code below though that was based on a older version
        //    UPGrid &grid = raster->gridRef();
        //    qint64 blockSizeBytes = grid->blockSize(block) * sizeof(T);
        //    qint64 szLeft = data.size();
        //    T value;
        //    QBuffer buf(&data);
        //    buf.open(QIODevice::ReadWrite);
        //    QDataStream stream(&buf);
        //    std::vector<double> values(grid->blockSize(0));
        //    quint32 noItems = grid->blockSize(block);
        //    if ( noItems == iUNDEF)
        //        return 0;

        //    values.resize(noItems);
        //    for(quint32 i=0; i < noItems; ++i) {
        //        stream >> value;

        //        values[i] = converter.raw2real(value);
        //    }

        //    grid->setBlockData(block, values);
        //    szLeft -= blockSizeBytes;
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

    for(int index = 0; index < raster->size().zsize(); ++index)   {
        const DataDefinition& def = raster->datadef(index);
        storeDataDefintion(def,_stream, options);

    }

    std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::IlwisVersion, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;
    domainStreamer->store( raster->stackDefinition().domain().ptr(), options);
    std::vector<QString> indexes = raster->stackDefinition().indexes();
    _stream << (quint32)indexes.size();
    for(auto index : indexes)
        _stream << index;

    std::unique_ptr<DataInterface> grfstreamer(factory->create(Version::IlwisVersion, itGEOREF,_stream));
    if ( !grfstreamer)
        return false;

    if(!grfstreamer->store(raster->georeference().ptr(), options))
        return false;
    _stream << raster->hasAttributes();
    if ( raster->hasAttributes()){
        std::unique_ptr<DataInterface> tblstreamer(factory->create(Version::IlwisVersion, itTABLE,_stream));
        if ( !tblstreamer)
            return false;

        if(!tblstreamer->store(raster->attributeTable().ptr(), options))
            return false;
    }
    return true;

}

bool RasterSerializerV1::storeData(IlwisObject *obj, const IOOptions &options )
{
    _stream << itRASTER;
    _stream << Version::IlwisVersion;
    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
    RawConverter converter;
    if ( hasType(raster->datadef().domain()->ilwisType() , itNUMERICDOMAIN)){
        NumericStatistics& stats = raster->statistics(ContainerStatistics<double>::pBASIC);
        qint16 digits = stats.significantDigits();
        double scale = std::pow(10,-digits);
        converter = RawConverter(stats[ContainerStatistics<double>::pMIN], stats[ContainerStatistics<double>::pMAX],scale);

        _stream << stats[ContainerStatistics<double>::pMIN] << stats[ContainerStatistics<double>::pMAX] << scale;
    }else{
        if(hasType(raster->datadef().domain()->ilwisType() ,itITEMDOMAIN) )
            converter = RawConverter("ident");
        if(hasType(raster->datadef().domain()->ilwisType() ,itCOLORDOMAIN) )
            converter = RawConverter("color");

    }
    if ( !converter.isValid()){
        kernel()->issues()->log(QString(TR("Couldnt find a correct converter for raster data of %1")).arg(obj->name()));
        return false;
    }
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
    IRasterCoverage rcoverage(raster);
    switch (converter.storeType()){
    case itUINT8:
        storeBulk<quint8>(converter, _stream, _streamconnector, box,rcoverage); break;
    case itINT16:
        storeBulk<qint16>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itUINT16:
        storeBulk<quint16>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itINT32:
        storeBulk<qint32>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itUINT32:
        storeBulk<quint32>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itDOUBLE:
        storeBulk<double>(converter, _stream, _streamconnector, box, rcoverage); break;
        break;
    case itINT64:
    default:
        for(double v : rcoverage)
            _stream << (qint64)v;
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

    std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::IlwisVersion, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;
    IDomain dom(type);
    domainStreamer->loadMetaData( dom.ptr(), options);
    quint32 nrOfBands;
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


    bool hasAttr;
    _stream >> hasAttr;
    if ( hasAttr){
        _stream >> type;
        _stream >> version;
        VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
        std::unique_ptr<DataInterface> tableStreamer(factory->create(version, itTABLE,_stream));
        if ( !tableStreamer)
            return false;
        FlatTable *tbl = new FlatTable();

        tableStreamer->loadMetaData(tbl,options);
        _stream >> type;
        _stream >> version;
        tableStreamer->loadData(tbl,options);
        raster->attributeTable(tbl);
    }

    return true;
}


bool RasterSerializerV1::loadData(IlwisObject *data, const IOOptions &options)
{
    RasterCoverage *raster = static_cast<RasterCoverage *>(data);
    const BoundingBox box;
    RawConverter converter;
    if ( hasType(raster->datadef().domain()->ilwisType(), itNUMERICDOMAIN)){
        double mmin, mmax, mscale;
        _stream >> mmin >> mmax >> mscale;
        converter = RawConverter(mmin, mmax, mscale);
    }else {
        if (  hasType(raster->datadef().domain()->ilwisType(), itITEMDOMAIN))
            converter = RawConverter("ident");
        if ( hasType(raster->datadef().domain()->ilwisType(), itCOLORDOMAIN))
            converter = RawConverter("color");
    }
    if ( !converter.isValid()){
        kernel()->issues()->log(QString(TR("Couldnt find a correct converter for raster data of %1")).arg(data->name()));
        return false;
    }

    quint32 layerIndex, minLines, maxLines; //only defined in some cases, if not defined it assumed that the whole coverage is there
    _stream >> layerIndex >> minLines >> maxLines;

    IRasterCoverage rcoverage(raster);
    switch (converter.storeType()){
    case itUINT8:
        loadBulk<quint8>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itINT16:
        loadBulk<qint16>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itUINT16:
        loadBulk<quint16>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itINT32:
        loadBulk<qint32>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itUINT32:
        loadBulk<quint32>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itDOUBLE:
        loadBulk<double>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itINT64:
    default:
            loadBulk<qint64>(converter, _stream, _streamconnector, box, rcoverage); break;
    }
    return true;
}

//quint32 RasterSerializerV1::loadGridBlock(IlwisObject *data, quint32 block, QByteArray &blockdata, const RawConverter& converter, const IOOptions &)
//{
//    RasterCoverage *raster = static_cast<RasterCoverage *>(data);

//    IRasterCoverage rcoverage(raster);
//    switch (converter.storeType()){
//    case itUINT8:
//        return loadBulk<quint8>(converter, block, blockdata, rcoverage); break;
//    case itINT16:
//        return loadBulk<qint16>(converter, block, blockdata, rcoverage); break;
//    case itINT32:
//        return loadBulk<qint32>(converter, block, blockdata, rcoverage); break;
//    case itINT64:{
//    default:
//            quint64 noOfPixels = raster->grid()->blockSize(block);
//            std::vector<double> values(noOfPixels);
//            for(int j = 0; j < noOfPixels; ++j){
//                _stream >> values[j];
//            }
//            raster->gridRef()->setBlockData(block, values);
//            return blockdata.size() - noOfPixels * 8;
//        }
//    }
//}

VersionedSerializer *RasterSerializerV1::create(QDataStream &stream)
{
    return new RasterSerializerV1(stream);
}
