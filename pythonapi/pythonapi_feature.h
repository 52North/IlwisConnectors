#ifndef PYTHONAPI_FEATURE_H
#define PYTHONAPI_FEATURE_H

#include "pythonapi_pyvariant.h"
#include "pythonapi_geometry.h"

#include <memory>

namespace Ilwis{    
    class FeatureInterface;
}

namespace pythonapi{

    class FeatureCoverage;

    class Feature: public Object{
        friend class Geometry;
    public:
        Feature(std::unique_ptr<Ilwis::FeatureInterface>& ilwisFeature, FeatureCoverage* fc);
        bool __bool__() const;
        const char* __str__();
        quint64 id();
        PyVariant* __getitem__(const char* name);
        PyVariant* __getitem__(quint32 colIndex);
        PyVariant* attribute(const char* name, PyVariant &defaultValue, int index = -1);
        PyVariant* attribute(const char* name, qlonglong defaultValue, int index = -1);
        PyVariant* attribute(const char* name, double defaultValue, int index = -1);
        PyVariant* attribute(const char* name, const char* defaultValue, int index = -1);
        void __setitem__(const char *name, PyVariant &value);
        void setAttribute(const char* name, PyVariant &value, int index = -1);
        void __setitem__(const char *name, qlonglong value);
        void setAttribute(const char* name, qlonglong value, int index = -1);
        void __setitem__(const char *name, double value);
        void setAttribute(const char* name, double value, int index = -1);
        void __setitem__(const char *name, const char *value);
        void setAttribute(const char* name, const char* value, int index = -1);
        IlwisTypes ilwisType();
        Geometry* geometry(int index = 0);
        void setGeometry(Geometry &geometry, int index = -1);

    private:
        //TODO: replace with std::shared_ptr of that feature if done in IlwisCore
        std::unique_ptr<Ilwis::FeatureInterface>& ptr() const;
        std::unique_ptr<Ilwis::FeatureInterface>& _ilwisSPFeatureI;
        FeatureCoverage* _coverage;
    };

}

#endif // PYTHONAPI_FEATURE_H
