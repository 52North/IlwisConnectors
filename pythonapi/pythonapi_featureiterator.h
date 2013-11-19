#ifndef PYTHONAPI_FEATUREITERATOR_H
#define PYTHONAPI_FEATUREITERATOR_H

#include "pythonapi_feature.h"
#include "pythonapi_featurecoverage.h"

namespace Ilwis {
    class FeatureIterator;
}

namespace pythonapi {

class FeatureIterator
{
public:
    FeatureIterator(FeatureCoverage* fc);
    ~FeatureIterator();
    /**
     * @brief next confusingly returns current value bevore iterating to the next item
     * brings together C++ style "std::iterator it != it.end()" with end() pointing the back (after last entry)
     * and Python style "for it in it:" using the __next__() also to retrieve first item.
     * @return
     */
    Feature next();
    bool hasNext();
private:
    Ilwis::FeatureIterator* _ilwisFeatureIterator;
    FeatureCoverage* _coverage;
};

}
#endif // PYTHONAPI_FEATUREITERATOR_H
