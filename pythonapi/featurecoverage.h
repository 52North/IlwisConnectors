#ifndef PYTHONAPI_FEATURECOVERAGE_H
#define PYTHONAPI_FEATURECOVERAGE_H

#include "coverage.h"

namespace pythonapi {

    class FeatureCoverage : public Coverage{
        public:
        FeatureCoverage();
        const char* toStr();
    };

}

#endif // PYTHONAPI_FEATURECOVERAGE_H
