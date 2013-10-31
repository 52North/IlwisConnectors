#ifndef PYTHONAPI_FEATURECOVERAGE_H
#define PYTHONAPI_FEATURECOVERAGE_H

#include "coverage.h"
#include "string"

namespace pythonapi {

    class FeatureCoverage : public Coverage{
    public:
        FeatureCoverage();
        FeatureCoverage(const char* resource);
        ~FeatureCoverage();
        const char* toStr();
        unsigned int featureCount() const;
        bool isValid();
    };
}

#endif // PYTHONAPI_FEATURECOVERAGE_H
