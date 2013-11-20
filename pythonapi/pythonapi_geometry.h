#ifndef PYTHONAPI_GEOMETRY_H
#define PYTHONAPI_GEOMETRY_H

#include "pythonapi_object.h"

#include <memory>

namespace Ilwis{
    class Geometry;
}

namespace pythonapi {

    class Geometry : public Object{
    public:
        Geometry(Ilwis::Geometry* geometry);
        ~Geometry();
        bool within(const Geometry& geometry) const;
        bool contains(const Geometry& geometry) const;
        bool __bool__() const ;
        const char* __str__();
        IlwisTypes ilwisType();
    private:
        std::unique_ptr<Ilwis::Geometry> _ilwisGeometry;
    };

} // namespace pythonapi

#endif // PYTHONAPI_GEOMETRY_H
