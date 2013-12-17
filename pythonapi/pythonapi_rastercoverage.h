#ifndef PYTHONAPI_RASTERCOVERAGE_H
#define PYTHONAPI_RASTERCOVERAGE_H

#include "pythonapi_coverage.h"
#include "pythonapi_geometry.h"
#include "pythonapi_pixeliterator.h"

namespace Ilwis {
    class RasterCoverage;
    typedef IlwisData<RasterCoverage> IRasterCoverage;
}

namespace pythonapi {

    class RasterCoverage : public Coverage{
         friend class PixelIterator;
         friend class Engine;
    private:
         RasterCoverage(Ilwis::IRasterCoverage* coverage);
    public:
        RasterCoverage();
        RasterCoverage(const char* resource);
        RasterCoverage* operator+ (RasterCoverage &rc);
        RasterCoverage* operator+ (double value);
        RasterCoverage* __radd__(double value);
        RasterCoverage* operator-(RasterCoverage &rc);
        RasterCoverage* operator- (double value);
        RasterCoverage* __rsub__(double value);
        RasterCoverage* operator* (RasterCoverage &rc);
        RasterCoverage* operator* (double value);
        RasterCoverage* __rmul__(double value);
        RasterCoverage* __truediv__ (RasterCoverage &rc);
        RasterCoverage* __truediv__ (double value);
        RasterCoverage* __rtruediv__(double value);
        PixelIterator __iter__();
        double coord2value(pythonapi::Coordinate &c);
        double pix2value(double x, double y, double z);

        Size size();
        void setSize(const Size& sz);
        void unloadBinary();

        static RasterCoverage* toRasterCoverage(Object *obj);
    };

}
#endif // PYTHONAPI_RASTERCOVERAGE_H
