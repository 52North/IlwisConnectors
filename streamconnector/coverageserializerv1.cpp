#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "coverage.h"
#include "coverageserializerv1.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"

using namespace Ilwis;
using namespace Stream;

CoverageSerializerV1::CoverageSerializerV1(QDataStream &stream) : VersionedSerializer(stream){

}

bool CoverageSerializerV1::store(IlwisObject *obj, int options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    std::unique_ptr<DataInterface> csyStreamer(factory->create(Version::IlwisVersion, itCOORDSYSTEM,_stream));
    if ( !csyStreamer)
        return false;
    Coverage *coverage = static_cast<Coverage *>(obj);
    csyStreamer->store(coverage->coordinateSystem().ptr(),options);

    _stream << coverage->envelope().min_corner().x << coverage->envelope().min_corner().y << coverage->envelope().max_corner().x << coverage->envelope().max_corner().y;

    std::unique_ptr<DataInterface> tblStreamer(factory->create(Version::IlwisVersion, itTABLE,_stream));
    if ( !tblStreamer)
        return false;

    if ( coverage->hasAttributes()){
        tblStreamer->store(coverage->attributeTable().ptr(), options);
    } else{
        _stream << itUNKNOWN;
    }

    if ( coverage->hasAttributes(Coverage::atINDEX)){
        tblStreamer->store(coverage->attributeTable(Coverage::atINDEX).ptr(), options);
        std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::IlwisVersion, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;
        domainStreamer->store(coverage->indexDefinition().domain().ptr(), options);
        coverage->indexDefinition().range()->store(_stream);
    }
    else
        _stream << itUNKNOWN;



    return true;
}

bool CoverageSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    Coverage *coverage = static_cast<Coverage *>(obj);
    quint64 type;
    QString version;
    _stream >> type;
    if ( type != itUNKNOWN){

        _stream >> version;

        std::unique_ptr<DataInterface> csyStreamer(factory->create(version, itCOORDSYSTEM,_stream));
        if ( !csyStreamer)
            return false;
        ICoordinateSystem csy(type);
        csyStreamer->loadMetaData(csy.ptr(),options);
        coverage->coordinateSystem(csy);
    }

    double minx, miny, maxx, maxy;
    _stream >> minx >> miny >> maxx >> maxy;
    coverage->envelope(Envelope(Coordinate(minx, miny), Coordinate(maxx, maxy)));

    std::unique_ptr<DataInterface> tableStreamer(factory->create(version, itTABLE,_stream));
    if ( !tableStreamer)
        return false;

    _stream >> type;
    if ( type != itUNKNOWN){
        _stream >> version;
        FlatTable *tbl = new FlatTable();
        tableStreamer->loadMetaData(tbl,options);
        coverage->attributeTable(tbl);
    }
    IlwisTypes valueType;
    _stream >> type;
    if ( type != itUNKNOWN){
        _stream >> version;
        FlatTable *tbl = new FlatTable();
        tableStreamer->loadMetaData(tbl,options);
        coverage->attributeTable(tbl,Coverage::atINDEX);

        std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::IlwisVersion, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;
        _stream >> valueType;
        _stream >> type;
        _stream >> version;
        IDomain dom(type | valueType);
        domainStreamer->loadMetaData(dom.ptr(),options);
        Range *range = Range::create(valueType);
        if ( !range)
            return false;
        range->load(_stream);
        coverage->indexDomain(dom, range);

    }

    return true;
}



