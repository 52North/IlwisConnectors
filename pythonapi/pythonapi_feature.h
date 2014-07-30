#ifndef PYTHONAPI_FEATURE_H
#define PYTHONAPI_FEATURE_H

#include "pythonapi_geometry.h"
#include "pythonapi_columndefinition.h"
#include <memory>

namespace Ilwis{    
    class FeatureInterface;
}

class QVariant;

typedef struct _object PyObject;



namespace pythonapi{

    class FeatureCoverage;
    const int COVERAGEATRIB = 100000000;
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
        PyObject* attribute(std::string name, qint64 defaultValue, int index = COVERAGEATRIB);
        PyObject* attribute(std::string name, double defaultValue, int index = COVERAGEATRIB);
        PyObject* attribute(std::string name, std::string defaultValue, int index = COVERAGEATRIB);
        void __setitem__(std::string name, const PyObject* value);
        void setAttribute(std::string name, const PyObject* value, int index = COVERAGEATRIB);
        void __setitem__(std::string name, qint64 value);
        void setAttribute(std::string name, qint64 value, int index = COVERAGEATRIB);
        void __setitem__(std::string name, double value);
        void setAttribute(std::string name, double value, int index = COVERAGEATRIB);
        void __setitem__(std::string name, std::string value);
        void setAttribute(std::string name, std::string value, int index = COVERAGEATRIB);
        IlwisTypes ilwisType();
        Geometry* geometry(int index = 0);
        void setGeometry(Geometry &geometry, int index = COVERAGEATRIB);
        pythonapi::ColumnDefinition columndefinition(const std::string& name, bool coverages=true) const;
        pythonapi::ColumnDefinition columndefinition(quint32 index, bool coverages=true) const;

    private:
        PyObject* attribute(std::string name, const QVariant& defaultValue, int index = COVERAGEATRIB);
        Feature(std::unique_ptr<Ilwis::FeatureInterface>& ilwisFeature, FeatureCoverage* fc);
        std::unique_ptr<Ilwis::FeatureInterface>& ptr() const;
        std::unique_ptr<Ilwis::FeatureInterface>& _ilwisSPFeatureI;
        FeatureCoverage* _coverage;
    };

}

#endif // PYTHONAPI_FEATURE_H
