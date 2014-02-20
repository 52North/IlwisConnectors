#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/geometries.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/georeference/georeference.h"

#include "pythonapi_georeference.h"

namespace pythonapi {

    GeoReference::GeoReference(Ilwis::IGeoReference* gr): IlwisObject(new Ilwis::IIlwisObject(*gr)){
    }

    GeoReference::GeoReference(const std::string& resource){
        Ilwis::IGeoReference gr;
        gr.prepare(QString::fromStdString(resource));
        if (gr.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(gr));
    }

    GeoReference* GeoReference::toGeoReference(Object *obj){
        GeoReference* ptr = dynamic_cast<GeoReference*>(obj);
        if(!ptr)
            throw InvalidObject("cast to GeoReference not possible");
        return ptr;
    }

    CoordinateSystem GeoReference::coordinateSystem() const{
        return CoordinateSystem(new Ilwis::ICoordinateSystem(this->ptr()->get<Ilwis::GeoReference>()->coordinateSystem()));
    }

    void GeoReference::setCoordinateSystem(const CoordinateSystem &csy){
        this->ptr()->get<Ilwis::GeoReference>()->coordinateSystem(csy.ptr()->get<Ilwis::CoordinateSystem>());
    }

    Coordinate GeoReference::pixel2Coord(const PixelD& pixel) const{
        return Coordinate(this->ptr()->get<Ilwis::GeoReference>()->pixel2Coord(pixel.data()));
    }

    Coordinate GeoReference::pixel2Coord(const Pixel& pixel) const{
        return Coordinate(this->ptr()->get<Ilwis::GeoReference>()->pixel2Coord(pixel.data()));
    }

    PixelD GeoReference::coord2Pixel(const Coordinate& coord) const{
        return PixelD(this->ptr()->get<Ilwis::GeoReference>()->coord2Pixel(coord.data()));
    }

    Envelope GeoReference::box2Envelope(const Box& box) const{
        return Envelope(this->ptr()->get<Ilwis::GeoReference>()->pixel2Coord(box.data()));
    }

    Box GeoReference::envelope2Box(const Envelope& box) const{
        return Box(this->ptr()->get<Ilwis::GeoReference>()->coord2Pixel(box.data()));
    }

    double GeoReference::pixelSize() const{
        return this->ptr()->get<Ilwis::GeoReference>()->pixelSize();
    }

    Size GeoReference::size() const{
        return Size(this->ptr()->get<Ilwis::GeoReference>()->size());
    }

    void GeoReference::size(const Size& sz){
        this->ptr()->get<Ilwis::GeoReference>()->size(sz.data());
    }

    bool GeoReference::centerOfPixel() const{
        return this->ptr()->get<Ilwis::GeoReference>()->centerOfPixel();
    }

    void GeoReference::centerOfPixel(bool yesno){
        this->ptr()->get<Ilwis::GeoReference>()->centerOfPixel(yesno);
    }

    bool GeoReference::isCompatible(const GeoReference& other) const{
        return this->ptr()->get<Ilwis::GeoReference>()->isCompatible(other.ptr()->get<Ilwis::GeoReference>());
    }

} // namespace pythonapi
