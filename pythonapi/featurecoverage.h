#ifndef PYTHONAPI_FEATURECOVERAGE_H
#define PYTHONAPI_FEATURECOVERAGE_H

#include "coverage.h"
#include "string"


namespace Ilwis {
    class FeatureCoverage;
    template<class T> class IlwisData;
    typedef IlwisData<Ilwis::FeatureCoverage> IFeatureCoverage;
}


namespace pythonapi {

    class FeatureCoverage : public Coverage{
    protected:
        Ilwis::IFeatureCoverage* _ilwisIFeatureCoverage;
    public:
        FeatureCoverage();
        FeatureCoverage(const char* resource);
        const char* toStr();
    };
}

#endif // PYTHONAPI_FEATURECOVERAGE_H
