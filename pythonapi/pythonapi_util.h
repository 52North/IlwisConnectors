#ifndef PYTHONAPI_UTIL_H
#define PYTHONAPI_UTIL_H

#include "pythonapi_ilwis.h"
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
    class Size;
}

namespace pythonapi {

    template<class CrdType> class PixelTemplate{
        public:
            PixelTemplate(const Ilwis::Location<CrdType, false>& pixel);
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

           Ilwis::Location<CrdType, false>& data() const;

            std::string __str__();
        protected:
            PixelTemplate(Ilwis::Location<CrdType, false>* pixel);
            std::shared_ptr<Ilwis::Location<CrdType, false> > _data;
    };

    typedef PixelTemplate<qint32> Pixel;
    typedef PixelTemplate<double> PixelD;

    class Coordinate{
        public:
            Coordinate(const Ilwis::Coordinate& coordinate);
            Coordinate(double x, double y);
            Coordinate(double x, double y, double z);
            double x() const;
            double y() const;
            double z() const;
            void setX(double v);
            void setY(double v);
            void setZ(double v);

           Ilwis::Coordinate& data() const;

            std::string __str__();
        protected:
            Coordinate(Ilwis::Coordinate* coordinate);
            std::shared_ptr<Ilwis::Coordinate> _data;
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

            bool __contains__(const Pixel& pix) const;
            std::string __str__();
            Ilwis::Size& data() const;
        private:
            std::shared_ptr<Ilwis::Size > _data;
        };

    /**
     * since the implementation of this template class is not included in the header file,
     * only the previously instanciated types (currently only Envelope and Box) can be used.
     */
    template<class IlwisType, class PyType> class BoxTemplate{
        public:
            BoxTemplate();
            BoxTemplate(const Ilwis::Box<IlwisType>& box);
            BoxTemplate(const std::string &envelope);
            BoxTemplate(const PyType& min,const PyType& max);
            BoxTemplate(const Size& size);
            BoxTemplate(const geos::geom::Envelope* envelope);
            std::string __str__();
            Size size();
            PyType minCorner();
            PyType maxCorner();
            bool contains(const PyType& point);
            bool contains(const BoxTemplate<IlwisType, PyType>& box);

            Ilwis::Box<IlwisType>& data() const;
    private:
            std::shared_ptr<Ilwis::Box<IlwisType> > _data;
    };
    typedef BoxTemplate<Ilwis::Coordinate, Coordinate> Envelope;
    typedef BoxTemplate<Ilwis::Location<qint32, false>, Pixel > Box;

} // namespace pythonapi

#endif // PYTHONAPI_UTIL_H
