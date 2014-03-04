#ifndef PYTHONAPI_FEATURE_H
#define PYTHONAPI_FEATURE_H

#include "pythonapi_geometry.h"

#include <memory>

namespace Ilwis{    
    class FeatureInterface;
}

class QVariant;

namespace pythonapi{

    class FeatureCoverage;

    class Feature: public Object{
        friend class Geometry;
        friend class FeatureCoverage;
        friend class FeatureIterator;
    public:
        bool __bool__() const;
        std::string __str__();
        quint64 id();
        PyObject* __getitem__(std::string name);
        PyObject* __getitem__(quint32 colIndex);
        PyObject* attribute(std::string name, qint64 defaultValue, int index = -1);
        PyObject* attribute(std::string name, double defaultValue, int index = -1);
        PyObject* attribute(std::string name, std::string defaultValue, int index = -1);
        void __setitem__(std::string name, const PyObject* value);
        void setAttribute(std::string name, const PyObject* value, int index = -1);
        void __setitem__(std::string name, qint64 value);
        void setAttribute(std::string name, qint64 value, int index = -1);
        void __setitem__(std::string name, double value);
        void setAttribute(std::string name, double value, int index = -1);
        void __setitem__(std::string name, std::string value);
        void setAttribute(std::string name, std::string value, int index = -1);
        IlwisTypes ilwisType();
        Geometry* geometry(int index = 0);
        void setGeometry(Geometry &geometry, int index = -1);

    private:
        PyObject* attribute(std::string name, const QVariant& defaultValue, int index = -1);
        Feature(std::unique_ptr<Ilwis::FeatureInterface>& ilwisFeature, FeatureCoverage* fc);
        std::unique_ptr<Ilwis::FeatureInterface>& ptr() const;
        std::unique_ptr<Ilwis::FeatureInterface>& _ilwisSPFeatureI;
        FeatureCoverage* _coverage;
    };

}

#endif // PYTHONAPI_FEATURE_H
