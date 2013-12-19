#ifndef PYTHONAPI_GEOMETRY_H
#define PYTHONAPI_GEOMETRY_H

#include "pythonapi_object.h"

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
        Geometry(std::string wkt);
        Geometry(Feature* feature, int index);
        ~Geometry();
        bool within(const Geometry& geometry) const;
        bool contains(const Geometry& geometry) const;
        bool __bool__() const ;
        std::string __str__();
        IlwisTypes ilwisType();
        bool fromWKT(const std::string& wkt);
        std::string toWKT();
    private:
        bool _standalone;

        Ilwis::Geometry &ptr() const;
        std::unique_ptr<Feature> _feature;
        int _index;

        std::string _wkt;
    };

    };

} // namespace pythonapi

#endif // PYTHONAPI_GEOMETRY_H
