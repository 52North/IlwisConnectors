#ifndef JAVAAPI_GEOREFERENCE_H
#define JAVAAPI_GEOREFERENCE_H

#include "javaapi_ilwisobject.h"
#include "javaapi_coordinatesystem.h"
#include "javaapi_util.h"

namespace Ilwis{
    class GeoReference;
    typedef IlwisData<GeoReference> IGeoReference;
}

namespace javaapi {

    class GeoReference : public IlwisObject{
        friend class Engine;
        friend class RasterCoverage;
        friend class Catalog;
        private:
            GeoReference(Ilwis::IGeoReference* gr);
        public:
            GeoReference(const std::string &resource);
            static GeoReference* toGeoReference(IObject *obj);

            CoordinateSystem coordinateSystem() const;
            void setCoordinateSystem(const CoordinateSystem& csy);

            Coordinate pixel2Coord(const PixelD& pixel) const;
            Coordinate pixel2Coord(const Pixel& pixel) const;
            PixelD coord2Pixel(const Coordinate& coord) const;
            virtual Envelope box2Envelope(const Box &box ) const;
            virtual Box envelope2Box(const Envelope &box) const;
            double pixelSize() const;

            Size size() const;
            void setSize(const Size& sz);
            bool centerOfPixel() const;
            void setCenterOfPixel(bool yesno);
            bool isCompatible(const GeoReference& other) const;
            bool compute();
            IlwisTypes ilwisType() const;

    };

} // namespace javaapi

#endif // JAVAAPI_GEOREFERENCE_H
