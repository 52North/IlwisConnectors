#ifndef PYTHONAPI_FEATURECOVERAGE_H
#define PYTHONAPI_FEATURECOVERAGE_H

#include "pythonapi_coverage.h"
#include "pythonapi_feature.h"
#include "pythonapi_geometry.h"
#include "pythonapi_featureiterator.h"

namespace Ilwis {
    class FeatureCoverage;
    typedef IlwisData<FeatureCoverage> IFeatureCoverage;
}

namespace pythonapi {

    class FeatureIterator;

    class FeatureCoverage : public Coverage{
        friend class FeatureIterator;
        friend class Engine;
        friend class Catalog;
    private:
        FeatureCoverage(Ilwis::IFeatureCoverage* coverage);
    public:
        FeatureCoverage();
        FeatureCoverage(std::string resource);
        FeatureIterator __iter__();
        IlwisTypes featureTypes() const;
        unsigned int featureCount() const;
        Feature newFeature(Geometry& geometry);
        static FeatureCoverage* toFeatureCoverage(Object *obj);
    };
}

#endif // PYTHONAPI_FEATURECOVERAGE_H
