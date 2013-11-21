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
        Geometry();
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


    class Coordinate : public Geometry{
    public:
        Coordinate(double x, double y, double z = -1e308);//rUNDEF = -1e308
        const char* __str__();
    private:
        bool _2d;
        double _x,_y,_z;
    };

    class Pixel : public Geometry{
    public:
        Pixel(qint32 x, qint32 y);
        const char* __str__();
    private:
        qint32 _x,_y;
    };
} // namespace pythonapi

#endif // PYTHONAPI_GEOMETRY_H
