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
        FeatureCoverage(const std::string& resource);
        FeatureIterator __iter__();
        IlwisTypes featureTypes() const;
        void featureTypes(IlwisTypes type);
        unsigned int featureCount() const;
        void setFeatureCount(IlwisTypes type, quint32 geomCnt, quint32 subGeomCnt, int index);
        Feature newFeature(const std::string& wkt, const CoordinateSystem& csy, bool load = true);
        Feature newFeature(const Geometry& geometry);
        Feature newFeatureFrom(const Feature& feat, const CoordinateSystem& csy);
        void reprojectFeatures(const CoordinateSystem& csy);
        static FeatureCoverage* toFeatureCoverage(Object *obj);
        PyObject* select(const std::string& spatialQuery);
        quint32 maxIndex() const;
        FeatureCoverage *clone();
        IlwisTypes geometryType(const Geometry& geom);
    };
}

#endif // PYTHONAPI_FEATURECOVERAGE_H
