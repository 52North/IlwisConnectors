#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "Eigen/LU"
#include "mathhelper.h"
#include "planarctpgeoreference.h"
#include "undeterminedgeoreference.h"
#include "connectorinterface.h"
#include "streamconnectorv1.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"
#include "streamcoveragedatainterfacev1.h"
#include "streamgeorefdatainterfacev1.h"

using namespace Ilwis;
using namespace Stream;

StreamGeorefDataInterfaceV1::StreamGeorefDataInterfaceV1(QDataStream& stream) : StreamConnectorV1(stream)
{
}


bool StreamGeorefDataInterfaceV1::store(IlwisObject *obj, int options)
{
    if (!StreamConnectorV1::store(obj, options))
        return false;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    GeoReference *grf = static_cast<GeoReference *>(obj);
    std::unique_ptr<DataInterface> csyStreamer(factory->create(Version::IlwisVersion, itCOORDSYSTEM,_stream));
    if ( !csyStreamer)
        return false;
    csyStreamer->store(grf->coordinateSystem().ptr(),options);

    _stream << grf->size().xsize() << grf->size().ysize() << grf->size().zsize();
    _stream << grf->centerOfPixel();

    if ( grf->grfType<CornersGeoReference>()){
        _stream << CornersGeoReference::typeName();
        Envelope envelope = grf->impl<CornersGeoReference>()->envelope();
        _stream << envelope.min_corner().x << envelope.min_corner().y << envelope.max_corner().x << envelope.max_corner().y;
    } else if ( grf->grfType<CTPGeoReference>()){
        _stream << CTPGeoReference::typeName();
        QSharedPointer<PlanarCTPGeoReference> ctpgrf = grf->impl<PlanarCTPGeoReference>();
        int nrOfControlPoints = ctpgrf->nrControlPoints();
        _stream << nrOfControlPoints;
        for(int i =0; i < nrOfControlPoints; ++i){
            _stream << ctpgrf->controlPoint(i).llLocation().x << ctpgrf->controlPoint(i).llLocation().y;
            _stream << ctpgrf->controlPoint(i).x << ctpgrf->controlPoint(i).y;
            _stream << ctpgrf->controlPoint(i).gridLocation().x << ctpgrf->controlPoint(i).gridLocation().y;
            _stream << ctpgrf->controlPoint(i).isActive();
        }
        _stream << (int)ctpgrf->transformation();
    } else if ( grf->grfType<UndeterminedGeoReference>()){
         _stream << UndeterminedGeoReference::typeName();
    }
    return true;

}

bool StreamGeorefDataInterfaceV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!StreamConnectorV1::loadMetaData(obj, options))
        return false;

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    quint64 type;
    QString version;
    _stream >> type;
    _stream >> version;

    std::unique_ptr<DataInterface> csyStreamer(factory->create(version, itCOORDSYSTEM,_stream));
    if ( !csyStreamer)
        return false;
    GeoReference *grf = static_cast<GeoReference *>(obj);
    ICoordinateSystem csy(type);
    csyStreamer->loadMetaData(csy.ptr(),options);


    quint32 xsize, ysize, zsize;
    _stream >> xsize >> ysize >> zsize;

    bool centerofpixels;
    _stream >> centerofpixels;

    QString typeName;
    _stream >> typeName;
    if ( typeName == CornersGeoReference::typeName()){
        double minx, miny, maxx, maxy;
        _stream >> minx >> miny >> maxx >> maxy;
        CornersGeoReference *cornersgrf = new CornersGeoReference();
        cornersgrf->setEnvelope(Envelope(Coordinate(minx, miny), Coordinate(maxx, maxy)));
        grf->impl(cornersgrf);
    } else if ( typeName == CTPGeoReference::typeName() ){
        PlanarCTPGeoReference *ctpgrf = new PlanarCTPGeoReference();
        int nrOfControlPoints;
        _stream >> nrOfControlPoints;
        for(int i = 0; i < nrOfControlPoints; ++i){
            ControlPoint pnt;
            double x,y;
            _stream >> x >> y;
            LatLon ll;
            ll.x = x;
            ll.y = y;
            pnt.llLocation(ll);
            _stream >> x >> y;
            pnt.x = x;
            pnt.y = y;
            _stream >> x >> y;
            Pixeld pix(x,y);
            pnt.gridLocation(pix);
            bool active;
            _stream >> active;
            pnt.active(active);;
            ctpgrf->setControlPoint(pnt);
        }
        int transformation;
        _stream >> transformation;
        ctpgrf->transformation((PlanarCTPGeoReference::Transformation)transformation);
        grf->impl(ctpgrf);
    }else{
        grf->impl( new UndeterminedGeoReference());
    }
    grf->size(Size<>(xsize, ysize, zsize));
    grf->coordinateSystem(csy);
    grf->centerOfPixel(centerofpixels);
    grf->compute();

    return true;

}

DataInterface *StreamGeorefDataInterfaceV1::create(QDataStream &stream)
{
    return new StreamGeorefDataInterfaceV1(stream);
}
