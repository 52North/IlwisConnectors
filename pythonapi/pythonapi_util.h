#ifndef PYTHONAPI_UTIL_H
#define PYTHONAPI_UTIL_H

#include "pythonapi_ilwis.h"
#include <memory>

namespace Ilwis{
    template<typename CrdType> class Point3D;
    template<class CsyType> class Box2D;
    template<class CsyType> class Box3D;
    class Size;
}

namespace pythonapi {

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
        void setX(qint32 v);
        void setY(qint32 v);
        void setZ(qint32 v);
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

#endif // PYTHONAPI_UTIL_H
