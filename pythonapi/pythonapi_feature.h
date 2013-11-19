#ifndef PYTHONAPI_FEATURE_H
#define PYTHONAPI_FEATURE_H

#include "pythonapi_pyvariant.h"

namespace Ilwis{
    class SPFeatureI;
}

namespace pythonapi{

    class FeatureCoverage;

    class Feature: public Object{
    public:
        Feature(Ilwis::SPFeatureI* ilwisFeature, FeatureCoverage* fc);
        bool __bool__() const;
        const char* __str__();
        PyVariant* attribute(const char* name, int index = -1);
        void attribute(const char* name, PyVariant &value, int index = -1);
        IlwisTypes ilwisType();

    private:
        //TODO: replace with std::shared_ptr of that feature if done in IlwisCore
        Ilwis::SPFeatureI ptr() const;
        Ilwis::SPFeatureI* _ilwisSPFeatureI;
        FeatureCoverage* _coverage;
    };

}

#endif // PYTHONAPI_FEATURE_H
