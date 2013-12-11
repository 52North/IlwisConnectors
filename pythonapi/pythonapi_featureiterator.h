#ifndef PYTHONAPI_FEATUREITERATOR_H
#define PYTHONAPI_FEATUREITERATOR_H

#include "pythonapi_feature.h"

namespace Ilwis {
    class FeatureIterator;
}

namespace pythonapi {

    class FeatureCoverage;

    class FeatureIterator{
        public:
            FeatureIterator(FeatureCoverage* fc);
            /**
             * @brief next confusingly returns current value bevore iterating to the next item
             * brings together C++ style "std::iterator it != it.end()" with end() pointing the back (after last entry)
             * and Python style "for it in it:" using the __next__() also to retrieve first item.
             * @return
             */
            Feature __next__();
            FeatureIterator* __iter__();
        private:
            FeatureCoverage* _coverage;
            std::shared_ptr<Ilwis::FeatureIterator> _ilwisFeatureIterator;
    };

}
#endif // PYTHONAPI_FEATUREITERATOR_H
