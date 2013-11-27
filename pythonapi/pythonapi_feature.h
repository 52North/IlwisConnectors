#ifndef PYTHONAPI_FEATURE_H
#define PYTHONAPI_FEATURE_H

#include "pythonapi_pyvariant.h"
#include "pythonapi_geometry.h"

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
        quint64 id();
        PyVariant* attribute(const char* name, int index = -1);
        PyVariant* attribute(const char* name, PyVariant &defaultValue, int index = -1);
        void setAttribute(const char* name, PyVariant &value, int index = -1);
        void setAttribute(const char* name, int value, int index = -1);
        void setAttribute(const char* name, uint value, int index = -1);
        void setAttribute(const char* name, qlonglong value, int index = -1);
        void setAttribute(const char* name, qulonglong value, int index = -1);
        void setAttribute(const char* name, bool value, int index = -1);
        void setAttribute(const char* name, double value, int index = -1);
        void setAttribute(const char* name, float value, int index = -1);
        void setAttribute(const char* name, const char* value, int index = -1);
        IlwisTypes ilwisType();
        Geometry* geometry(int index = 0);
        void setGeometry(Geometry &geometry, int index = -1);

    private:
        //TODO: replace with std::shared_ptr of that feature if done in IlwisCore
        Ilwis::SPFeatureI ptr() const;
        Ilwis::SPFeatureI* _ilwisSPFeatureI;
        FeatureCoverage* _coverage;
    };

}

#endif // PYTHONAPI_FEATURE_H
