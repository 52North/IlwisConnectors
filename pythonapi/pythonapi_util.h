#ifndef PYTHONAPI_UTIL_H
#define PYTHONAPI_UTIL_H

#include "pythonapi_ilwis.h"
#include <memory>

namespace Ilwis{
    template<typename CrdType> class Point2D;
    template<typename CrdType> class Point3D;
    template<class CsyType> class Box3D;
    class Size;
}

namespace pythonapi {

    template<class T> class Point2DTemplate{
    public:
        Point2DTemplate(const Ilwis::Point2D<T>& point);
        Point2DTemplate(T x, T y);
        T x() const;
        T y() const;
        void setX(T v);
        void setY(T v);

        Ilwis::Point2D<T>& data() const;

        //implemented in spezialized templates only
        std::string __str__();

    private:
        std::shared_ptr<Ilwis::Point2D<T> > _data;
    };

    template<> std::string Point2DTemplate<qint32>::__str__();
    template<> std::string Point2DTemplate<double>::__str__();

    typedef Point2DTemplate<double> Coordinate2D;
    typedef Point2DTemplate<qint32> Pixel;


    template<class T> class Point3DTemplate{
    public:
        Point3DTemplate(const Ilwis::Point3D<T>& point);
        Point3DTemplate(T x, T y, T z);
        T x() const;
        T y() const;
        T z() const;
        void setX(T v);
        void setY(T v);
        void setZ(T v);

        Ilwis::Point3D<T>& data() const;

        //implemented in spezialized templates only
        std::string __str__();
    private:
        std::shared_ptr<Ilwis::Point3D<T> > _data;
    };

    template<> std::string Point3DTemplate<qint32>::__str__();
    template<> std::string Point3DTemplate<double>::__str__();

    typedef Point3DTemplate<double> Coordinate;
    typedef Point3DTemplate<qint32> Voxel;

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

    /**
     * since the implementation of this template class is not included in the header file,
     * only the previously instanciated types (currently only Envelope and Box) can be used.
     */
    template<class T> class BoxTemplate{
    public:
        BoxTemplate();
        BoxTemplate(const Ilwis::Box3D<T>& box);
        BoxTemplate(const std::string &envelope);
        BoxTemplate(const Voxel& min,const Voxel& max);
        BoxTemplate(const Coordinate& min,const Coordinate& max);
        std::string __str__();
        Size size();
        Ilwis::Box3D<T>& data() const;
    private:
        std::shared_ptr<Ilwis::Box3D<T> > _data;
    };
    typedef BoxTemplate<double> Envelope;
    typedef BoxTemplate<qint32> Box;

} // namespace pythonapi

#endif // PYTHONAPI_UTIL_H
