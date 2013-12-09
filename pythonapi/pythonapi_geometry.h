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
        Geometry(const char* wkt);
        Geometry(Feature* feature, int index);
        ~Geometry();
        bool within(const Geometry& geometry) const;
        bool contains(const Geometry& geometry) const;
        bool __bool__() const ;
        const char* __str__();
        IlwisTypes ilwisType();
        bool fromWKT(const char*  wkt);
        const char* toWKT();
    private:
        bool _standalone;

        Ilwis::Geometry &ptr() const;
        std::unique_ptr<Feature> _feature;
        int _index;

        std::string _wkt;
    };


    //input helper classes: no IlwisObject management

    class Coordinate{
    public:
        Coordinate(double x, double y, double z = -1e308);//rUNDEF = -1e308
        const char* __str__();
    private:
        bool _2d;
        double _x,_y,_z;
    };

    class Pixel{
    public:
        Pixel(qint32 x, qint32 y);
        const char* __str__();
    private:
        qint32 _x,_y;
    };


} // namespace pythonapi

#endif // PYTHONAPI_GEOMETRY_H
