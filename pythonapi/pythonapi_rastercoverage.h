#ifndef PYTHONAPI_RASTERCOVERAGE_H
#define PYTHONAPI_RASTERCOVERAGE_H

#include "pythonapi_coverage.h"

namespace Ilwis {
    class RasterCoverage;
    typedef IlwisData<RasterCoverage> IRasterCoverage;
}

namespace pythonapi {

    class RasterCoverage : public Coverage{
    public:
        RasterCoverage();
        RasterCoverage(const char* resource);
        RasterCoverage* __add__(RasterCoverage &rc);
        RasterCoverage* __add__(double value);
        RasterCoverage* __radd__(double value);
        RasterCoverage* __sub__(RasterCoverage &rc);
        RasterCoverage* __sub__(double value);
        RasterCoverage* __rsub__(double value);
        RasterCoverage* __mul__(RasterCoverage &rc);
        RasterCoverage* __mul__(double value);
        RasterCoverage* __rmul__(double value);
        RasterCoverage* __truediv__(RasterCoverage &rc);
        RasterCoverage* __truediv__(double value);
        RasterCoverage* __rtruediv__(double value);
        double value(double x, double y, double z);
        static RasterCoverage* toRasterCoverage(Object *obj);
    };

}
#endif // PYTHONAPI_RASTERCOVERAGE_H
