#ifndef PYTHONAPI_RASTERCOVERAGE_H
#define PYTHONAPI_RASTERCOVERAGE_H

#include "coverage.h"

namespace Ilwis {
    class RasterCoverage;
    typedef IlwisData<RasterCoverage> IRasterCoverage;
}

namespace pythonapi {

    class RasterCoverage : public Coverage{
    public:
        RasterCoverage();
    };

}
#endif // PYTHONAPI_RASTERCOVERAGE_H
