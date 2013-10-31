#ifndef PYTHONAPI_FEATUREITERATOR_H
#define PYTHONAPI_FEATUREITERATOR_H

#include "feature.h"
#include "featurecoverage.h"

namespace Ilwis {
    class FeatureIterator;
}

namespace pythonapi {

class FeatureIterator
{
public:
    FeatureIterator(FeatureCoverage fc);
    Feature next();
private:
    Ilwis::FeatureIterator* _ilwisFeatureIterator;
};

}
#endif // PYTHONAPI_FEATUREITERATOR_H
