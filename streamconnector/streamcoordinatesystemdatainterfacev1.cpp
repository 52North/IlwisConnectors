#include "kernel.h"
#include "version.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "projection.h"
#include "geodeticdatum.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "streamconnectorv1.h"
#include "versioneddatastreamfactory.h"
#include "streamcoordinatesystemdatainterfacev1.h"


using namespace Ilwis;
using namespace Stream;

DataInterface *StreamCoordinateSystemDataInterfaceV1::create(QDataStream& stream)
{
    return new StreamCoordinateSystemDataInterfaceV1(stream);
}

StreamCoordinateSystemDataInterfaceV1::StreamCoordinateSystemDataInterfaceV1(QDataStream& stream) : StreamConnectorV1(stream)
{
}

bool StreamCoordinateSystemDataInterfaceV1::store(IlwisObject *obj, int options)
{
    if (!StreamConnectorV1::store(obj, options))
        return false;

    CoordinateSystem *csy = static_cast<CoordinateSystem *>(obj);
    if ( csy->ilwisType() == itCONVENTIONALCOORDSYSTEM){
        VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
        if (!factory)
            return false;
        std::unique_ptr<DataInterface> projstreamer(factory->create(Version::IlwisVersion, itPROJECTION,_stream));
        if ( !projstreamer)
            return false;
        ConventionalCoordinateSystem *ccsy = static_cast<ConventionalCoordinateSystem *>(csy);
        projstreamer->store(ccsy->projection().ptr(),options);
        std::unique_ptr<DataInterface> ellstreamer(factory->create(Version::IlwisVersion, itELLIPSOID,_stream));
        if ( !ellstreamer)
            return false;
        ellstreamer->store(ccsy->ellipsoid().ptr(),options);

        const std::unique_ptr<GeodeticDatum>& datum = ccsy->datum();
        if ( datum){
            _stream << itGEODETICDATUM << datum->name() << datum->code() << datum->description() << datum->area() << datum->authority();
            for(int i =0; i < 10; ++i)
                _stream << datum->parameter((GeodeticDatum::DatumParameters)i);
        }else
            _stream << itUNKNOWN;

        _stream << ccsy->unit();

    }
    _stream << csy->envelope().min_corner().x << csy->envelope().min_corner().y << csy->envelope().max_corner().x << csy->envelope().max_corner().y;
    return true;

}

bool StreamCoordinateSystemDataInterfaceV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!StreamConnectorV1::loadMetaData(obj, options))
        return false;
    CoordinateSystem *csy = static_cast<CoordinateSystem *>(obj);
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    if ( csy->ilwisType() == itCONVENTIONALCOORDSYSTEM){
        ConventionalCoordinateSystem *convCsy = static_cast<ConventionalCoordinateSystem *>(obj);
        quint64 type;
        QString version;
        _stream >> type;
        if ( type != itUNKNOWN){
            _stream >> version;
            std::unique_ptr<DataInterface> projstreamer(factory->create(version, type,_stream));
            if ( !projstreamer)
                return false;
            Projection *proj = new Projection();
            projstreamer->loadMetaData(proj, options );
            convCsy->setProjection(proj);
        }
        _stream >> type;
        if ( type != itUNKNOWN){
            _stream >> version;

            std::unique_ptr<DataInterface> ellstreamer(factory->create(Version::IlwisVersion, itELLIPSOID,_stream));
            if ( !ellstreamer)
                return false;
            Ellipsoid *ellipsoid = new Ellipsoid();
            ellstreamer->loadMetaData(ellipsoid, options);
            convCsy->setEllipsoid(ellipsoid);
        }
        _stream >> type;
        if ( type != itUNKNOWN){
            QString name, code, description, area, authority;
            _stream >> name >> code >> description >> area >> authority;
            GeodeticDatum *datum = new GeodeticDatum(name);
            datum->setArea(area);
            datum->code(code);
            datum->setDescription(description);
            datum->setAuthority(authority);
            double values[10];
            for(int i =0; i < 10; ++i){
                _stream >> values[i];
            }
            datum->set10TransformationParameters(values[0],values[1], values[2], values[3], values[4], values[5], values[6],Ilwis::Coordinate(values[7], values[8], values[9]));
            convCsy->setDatum(datum);

        }
        QString unit;
        _stream >> unit;
        convCsy->unit(unit);

    }
    double minx, miny, maxx, maxy;
    _stream >> minx >> miny >> maxx >> maxy;
    csy->envelope(Envelope(Coordinate(minx, miny), Coordinate(maxx, maxy)));

    return true;
}
