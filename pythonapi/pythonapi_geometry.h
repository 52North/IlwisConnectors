#ifndef PYTHONAPI_GEOMETRY_H
#define PYTHONAPI_GEOMETRY_H

#include "pythonapi_object.h"

#include <memory>

namespace Ilwis{
    class Geometry;
    class SPFeatureI;
    template<typename CrdType> class Point3D;
    template<class CsyType> class Box2D;
    template<class CsyType> class Box3D;
    class Size;
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


    //input helper classes: no IlwisObject management

    class Coordinate{
    public:
        Coordinate(double x, double y);
        Coordinate(double x, double y, double z);
        std::string __str__();
        Ilwis::Point3D<double>& data();
    private:
        bool _2d;
        std::shared_ptr<Ilwis::Point3D<double> > _data;
    };

    class Voxel{
    public:
        Voxel(const Ilwis::Point3D<qint32>& vox);
        Voxel(qint32 x, qint32 y, qint32 z);
        qint32 x() const;
        qint32 y() const;
        qint32 z() const;
        std::string __str__();
        Ilwis::Point3D<qint32>& data() const;
    private:
        std::shared_ptr<Ilwis::Point3D<qint32> > _data;
    };

    class Pixel{
    public:
        Pixel(qint32 x, qint32 y);
        std::string __str__();
    private:
        qint32 _x,_y;
    };

    class Size{
    public:
        Size(qint32 xsize, qint32 ysize, qint32 zsize=1);
        Size(const Ilwis::Size& size);

        Size* operator+=(const Size& sz);
        Size* operator-=(const Size& sz);
        Size* operator*=(double f);
        bool operator==(const Size& sz);
        qint32 xsize() const;
        qint32 ysize() const;
        qint32 zsize() const;
        void setXsize(qint32 x);
        void setYsize(qint32 y);
        void setZsize(qint32 z);
        quint64 linearSize() const;

        bool __contains__(const Voxel& vox) const;
        std::string __str__();
        Ilwis::Size& data() const;
    private:
        std::shared_ptr<Ilwis::Size > _data;
    };

    class Box{
    public:
        Box();
        Box(const Ilwis::Box3D<qint32>& box);
        Box(const std::string &envelope);
        Box(const Voxel& min,const Voxel& max);
        std::string __str__();
        Size size();
        Ilwis::Box3D<qint32>& data() const;
    private:
        std::shared_ptr<Ilwis::Box3D<qint32> > _data;
    };

} // namespace pythonapi

#endif // PYTHONAPI_GEOMETRY_H
