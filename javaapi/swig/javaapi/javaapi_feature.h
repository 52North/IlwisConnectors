#ifndef JAVAAPI_FEATURE_H
#define JAVAAPI_FEATURE_H

#include "javaapi_geometry.h"
#include "javaapi_columndefinition.h"
#include <memory>

namespace Ilwis{    
    class FeatureInterface;
    class SPFeatureI;
}

class QVariant;

namespace javaapi{

    class FeatureCoverage;
    const int COVERAGEATRIB = 100000000;
    class Feature: public IObject{
        friend class Geometry;
        friend class FeatureCoverage;
        friend class FeatureIterator;
    public:
        bool __bool__() const;
        std::string __str__();
        VertexIterator __iter__();

        //Feature createSubFeature(PyObject* subFeatureIndex, const Geometry& geom) ;

        qint64 attribute(const std::string& name, qint64 defaultValue);
        double attribute(const std::string& name, double defaultValue);
        std::string attribute(const std::string& name, const std::string& defaultValue);
        void setAttribute(const std::string& name, const QVariant* value);
        void setAttribute(const std::string& name, qint64 value);
        void setAttribute(const std::string& name, double value);
        void setAttribute(const std::string& name, const std::string& value);
        IlwisTypes ilwisType();

        Geometry* geometry();
        void geometry(const Geometry& geom);
        IlwisTypes geometryType();

        quint64 featureId() const;

        javaapi::ColumnDefinition attributeDefinition(const std::string& name) const;
        javaapi::ColumnDefinition attributeDefinition(quint32 index) const;
        quint32 attributeColumnCount() const;

        void removeSubFeature(const std::string& subFeatureIndex);
        void setSubFeature(const std::string &subFeatureIndex, Feature feature);
        void removeSubFeature(double subFeatureIndex);
        void setSubFeature(double subFeatureIndex, Feature feature);
        quint32 subFeatureCount() const;

        //void setRecord(QVariant* pyValues, quint32 offset = 0);
        //QVariant *record();

        VertexIterator begin();
        VertexIterator end();

    private:
        QVariant attribute(const std::string& name, const QVariant& defaultValue);
        QVariant* checkIndex(QVariant* obj);
        Feature(std::shared_ptr<Ilwis::FeatureInterface> ilwisFeature, FeatureCoverage* fc);
        Feature(Ilwis::SPFeatureI* ilwFeat, FeatureCoverage* fc);
        std::shared_ptr<Ilwis::FeatureInterface> ptr() const;
        std::shared_ptr<Ilwis::FeatureInterface> _ilwisSPFeatureI;
        FeatureCoverage* _coverage;
    };

}

#endif // JAVAAPI_FEATURE_H
