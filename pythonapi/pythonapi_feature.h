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
        std::string __str__();
        quint64 id();
        PyVariant* __getitem__(std::string name);
        PyVariant* __getitem__(quint32 colIndex);
        PyVariant* attribute(std::string name, PyVariant &defaultValue, int index = -1);
        PyVariant* attribute(std::string name, qlonglong defaultValue, int index = -1);
        PyVariant* attribute(std::string name, double defaultValue, int index = -1);
        PyVariant* attribute(std::string name, std::string defaultValue, int index = -1);
        void __setitem__(std::string name, PyVariant &value);
        void setAttribute(std::string name, PyVariant &value, int index = -1);
        void __setitem__(std::string name, qlonglong value);
        void setAttribute(std::string name, qlonglong value, int index = -1);
        void __setitem__(std::string name, double value);
        void setAttribute(std::string name, double value, int index = -1);
        void __setitem__(std::string name, std::string value);
        void setAttribute(std::string name, std::string value, int index = -1);
        IlwisTypes ilwisType();
        Geometry* geometry(int index = 0);
        void setGeometry(Geometry &geometry, int index = -1);

    private:
        std::unique_ptr<Ilwis::FeatureInterface>& ptr() const;
        std::unique_ptr<Ilwis::FeatureInterface>& _ilwisSPFeatureI;
        FeatureCoverage* _coverage;
    };

}

#endif // PYTHONAPI_FEATURE_H
