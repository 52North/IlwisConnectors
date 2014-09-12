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
        VertexIterator __iter__();
        quint64 id();
        PyObject* __getitem__(std::string name);
        PyObject* __getitem__(quint32 colIndex);
        PyObject* attribute(std::string name, qint64 defaultValue, PyObject* index = NULL);
        PyObject* attribute(std::string name, double defaultValue, PyObject* index = NULL);
        PyObject* attribute(std::string name, std::string defaultValue, PyObject* index = NULL);
        void __setitem__(std::string name, const PyObject* value);
        void setAttribute(std::string name, const PyObject* value, PyObject* index = NULL);
        void __setitem__(std::string name, qint64 value);
        void setAttribute(std::string name, qint64 value, PyObject* index = NULL);
        void __setitem__(std::string name, double value);
        void setAttribute(std::string name, double value, PyObject* index = NULL);
        void __setitem__(std::string name, std::string value);
        void setAttribute(std::string name, std::string value, PyObject* index = NULL);
        IlwisTypes ilwisType();
        Geometry* geometry(PyObject* index = NULL);
        void setGeometry(Geometry &geometry, PyObject* index = NULL);
        void addGeometry(Geometry &geometry, PyObject* index = NULL);
        void removeGeometry(PyObject* index);
        quint64 featureId() const;
        quint32 trackSize() const;
        pythonapi::ColumnDefinition columnDefinition(const std::string& name, bool coverages=true) const;
        pythonapi::ColumnDefinition columnDefinition(quint32 index, bool coverages=true) const;
        PyObject* trackIndexValue(quint32 index);

        VertexIterator begin();
        VertexIterator end();

    private:
        PyObject* attribute(std::string name, const QVariant& defaultValue, const QVariant& index = COVERAGEATRIB);
        QVariant* checkIndex(PyObject* obj);
        Feature(std::unique_ptr<Ilwis::FeatureInterface>& ilwisFeature, FeatureCoverage* fc);
        std::unique_ptr<Ilwis::FeatureInterface>& ptr() const;
        std::unique_ptr<Ilwis::FeatureInterface>& _ilwisSPFeatureI;
        FeatureCoverage* _coverage;
    };

}

#endif // PYTHONAPI_FEATURE_H
