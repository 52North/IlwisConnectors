#ifndef PYTHONAPI_UTIL_H
#define PYTHONAPI_UTIL_H

#include "pythonapi_extension.h"
#include <memory>

namespace geos {
namespace geom {
    class Envelope;
}
}

namespace Ilwis{
    template<class CrdType, bool useDouble> class Location;
    class Coordinate;
    template<class CrdType> class Box;
    template<typename T> class Size;
}

namespace pythonapi {
/**
 * since the implementation of this template classes are not included in the header file,
 * only the previously instanciated types (currently only Pixel, PixelD, Envelope, Box, Size and SizeD) can be used.
 */
    class Coordinate;

    template<class CrdType> class PixelTemplate{
        template<class IlwisType, class PyType, typename T> friend class BoxTemplate;
        template<typename T> friend class SizeTemplate;
        friend class PixelIterator;
        friend class GeoReference;
        friend class RasterCoverage;
        public:
            PixelTemplate(const PixelTemplate<qint32>& pixel);
            PixelTemplate(const PixelTemplate<double>& pixel);
            PixelTemplate(CrdType x, CrdType y);
            PixelTemplate(CrdType x, CrdType y, CrdType z);
            CrdType x() const;
            CrdType y() const;
            CrdType z() const;
            void setX(CrdType v);
            void setY(CrdType v);
            void setZ(CrdType v);
            bool is3D() const;

            PixelTemplate<CrdType>* operator *=(CrdType n);
            PixelTemplate<CrdType>* __itruediv__(CrdType n);
            bool operator==(const PixelTemplate<CrdType>& other);
            bool operator!=(const PixelTemplate<CrdType>& other);

            std::string __str__();
            bool __bool__() const;
        private:
            PixelTemplate(const Ilwis::Location<CrdType, false>& pixel);
            PixelTemplate(Ilwis::Location<CrdType, false>* pixel);
            Ilwis::Location<CrdType, false>& data() const;
            std::shared_ptr<Ilwis::Location<CrdType, false> > _data;
    };

    typedef PixelTemplate<qint32> Pixel;
    typedef PixelTemplate<double> PixelD;

    class Coordinate{
        template<class IlwisType, class PyType, typename T> friend class BoxTemplate;
        template<typename T> friend class SizeTemplate;
        friend class RasterCoverage;
        friend class GeoReference;
        public:
            Coordinate(double x, double y);
            Coordinate(double x, double y, double z);
            Coordinate(const Coordinate& crd);
            double x() const;
            double y() const;
            double z() const;
            void setX(double v);
            void setY(double v);
            void setZ(double v);
            bool is3D() const;

            Coordinate* operator*=(double n);
            Coordinate* __itruediv__(double n);
            bool operator==(const Coordinate& other);
            bool operator!=(const Coordinate& other);

            std::string __str__();
            bool __bool__() const;
        private:
            Coordinate(const Ilwis::Coordinate& coordinate);
            Coordinate(Ilwis::Coordinate* coordinate);
            Ilwis::Coordinate& data() const;
            std::shared_ptr<Ilwis::Coordinate> _data;
    };

    template<typename T> class SizeTemplate{
        template<class IlwisType, class PyType, typename U> friend class BoxTemplate;
        friend class RasterCoverage;
        friend class GeoReference;
        public:
            SizeTemplate(T xsize, T ysize, T zsize=1);
            SizeTemplate(const SizeTemplate<quint32>& size);
            SizeTemplate(const SizeTemplate<double>& size);

            T xsize() const;
            T ysize() const;
            T zsize() const;
            void setXsize(T x);
            void setYsize(T y);
            void setZsize(T z);
            quint64 linearSize() const;

            SizeTemplate<T>* operator+=(const SizeTemplate<T>& sz);
            SizeTemplate<T>* operator-=(const SizeTemplate<T>& sz);
            SizeTemplate<T>* operator*=(double f);
            bool operator==(const SizeTemplate<T>& sz);
            bool operator!=(const SizeTemplate<T>& sz);

            bool __contains__(const Coordinate& pix) const;
            bool __contains__(const PixelTemplate<qint32>& pix) const;
            bool __contains__(const PixelTemplate<double>& pix) const;

            std::string __str__();
            bool __bool__() const;
        private:
            SizeTemplate(const Ilwis::Size<T>& size);
            Ilwis::Size<T>& data() const;
            std::shared_ptr<Ilwis::Size<T> > _data;
    };
    typedef SizeTemplate<quint32> Size;
    typedef SizeTemplate<double> SizeD;

    template<class IlwisType, class PyType, typename SizeType> class BoxTemplate{
        friend class PixelIterator;
        friend class GeoReference;
        friend class CoordinateSystem;
        friend class Geometry;
        public:
            BoxTemplate();
            BoxTemplate(const std::string &envelope);
            BoxTemplate(const PyType& min,const PyType& max);
            BoxTemplate(const SizeTemplate<SizeType>& size);

            PyType minCorner();
            PyType maxCorner();
            SizeTemplate<SizeType> size();

            bool is3D() const;
            bool __contains__(const PyType& point) const;
            bool __contains__(const BoxTemplate<IlwisType, PyType, SizeType>& box) const;
            bool operator==(const BoxTemplate<IlwisType, PyType, SizeType>& other);
            bool operator!=(const BoxTemplate<IlwisType, PyType, SizeType>& other);

            std::string __str__();
            bool __bool__() const;
        private:
            BoxTemplate(const Ilwis::Box<IlwisType>& box);
            BoxTemplate(const geos::geom::Envelope* envelope);
            Ilwis::Box<IlwisType>& data() const;
            std::shared_ptr<Ilwis::Box<IlwisType> > _data;
    };
    typedef BoxTemplate<Ilwis::Coordinate, Coordinate, double> Envelope;
    typedef BoxTemplate<Ilwis::Location<qint32, false>, Pixel, quint32 > Box;

} // namespace pythonapi

#endif // PYTHONAPI_UTIL_H
