#ifndef PYTHONAPI_FEATURECOVERAGE_H
#define PYTHONAPI_FEATURECOVERAGE_H

#include "coverage.h"

namespace Ilwis {
    class FeatureCoverage;
    typedef IlwisData<FeatureCoverage> IFeatureCoverage;
}

namespace pythonapi {

    class FeatureCoverage : public Coverage{
    public:
        FeatureCoverage();
        FeatureCoverage(const char* resource);
        ~FeatureCoverage();
        unsigned int featureCount() const;
    };
}

#endif // PYTHONAPI_FEATURECOVERAGE_H
