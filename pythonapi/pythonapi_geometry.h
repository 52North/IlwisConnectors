#ifndef PYTHONAPI_GEOMETRY_H
#define PYTHONAPI_GEOMETRY_H

#include "pythonapi_object.h"
#include "pythonapi_coordinatesystem.h"

#include <memory>

namespace Ilwis{
    class Geometry;
    class SPFeatureI;
}

namespace pythonapi {

    class Feature;

    class Geometry : public Object{
        friend class Feature;
    public:
        Geometry(std::string wkt, const CoordinateSystem& cs);
        Geometry(Feature* feature, int index);
        Geometry(Ilwis::Geometry* geometry);
        ~Geometry();
        bool within(const Geometry& geometry) const;
        bool contains(const Geometry& geometry) const;
        bool __bool__() const ;
        std::string __str__();
        IlwisTypes ilwisType();
        bool fromWKT(const std::string& wkt);
        std::string toWKT();
        CoordinateSystem coordinateSystem();
        void setCoordinateSystem(const CoordinateSystem& cs);
        Geometry* transform(const CoordinateSystem& cs);
        Envelope envelope();
    private:
        Ilwis::Geometry &ptr() const;
        bool _standalone;

        //_standalone == false
        std::unique_ptr<Feature> _feature;
        int _index;

        //_standalone == true
        std::unique_ptr<Ilwis::Geometry> _ilwisGeometry;
    };

} // namespace pythonapi

#endif // PYTHONAPI_GEOMETRY_H
