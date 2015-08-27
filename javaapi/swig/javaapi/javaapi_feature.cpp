#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/util/angle.h"

#include "geos/geom/Geometry.h"
#include "geos/geom/Coordinate.h"
#include "../../IlwisCore/core/util/location.h"
#include "../../IlwisCore/core/util/coordinate.h"
#include "../../IlwisCore/core/util/box.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributedefinition.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/geometryhelper.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/vertexiterator.h"

#include "javaapi_feature.h"
#include "javaapi_featurecoverage.h"
#include "javaapi_columndefinition.h"
#include "javaapi_vertexiterator.h"
#include <QVariant>



using namespace javaapi;

Feature::Feature(std::shared_ptr<Ilwis::FeatureInterface> ilwisFeature, FeatureCoverage* fc): _ilwisSPFeatureI(ilwisFeature), _coverage(fc){
}

Feature::Feature(Ilwis::SPFeatureI* ilwFeat, FeatureCoverage* fc): _ilwisSPFeatureI(*ilwFeat), _coverage(fc){
}

bool Feature::__bool__() const{
    return  (bool)this->_ilwisSPFeatureI && this->_ilwisSPFeatureI->isValid() && this->_coverage != NULL && this->_coverage->__bool__();
}

std::string Feature::__str__(){
    if (this->__bool__())
        return QString("Feature(%1)").arg(this->_ilwisSPFeatureI->featureid()).toStdString();
    else
        return QString("invalid Feature!").toStdString();
}

VertexIterator Feature::__iter__(){
    Geometry* geom = this->geometry();
    return VertexIterator(*geom);
}

/*Feature Feature::createSubFeature(PyObject* subFeatureIndex, const Geometry &geom){
    Ilwis::SPFeatureI ilwFeat = Ilwis::SPFeatureI();
    if(PyFloatCheckExact(subFeatureIndex)){ // float
            double val = PyFloatAsDouble(subFeatureIndex);
            std::string valStr = std::to_string(val);
            ilwFeat = this->ptr()->createSubFeature(QString::fromStdString(valStr), geom.ptr().get()->clone());
    }else if(PyUnicodeCheckExact(subFeatureIndex)){ // string
            std::string val = PyBytesAsString(subFeatureIndex);
            ilwFeat = this->ptr()->createSubFeature(QString::fromStdString(val), geom.ptr().get()->clone());
    }else if(PyDateTimeCheckExact(subFeatureIndex) || PyDateCheckExact(subFeatureIndex) || PyTimeCheckExact(subFeatureIndex)){ // time
            int year = PyDateTimeGET_YEAR(subFeatureIndex);
            int month = PyDateTimeGET_MONTH(subFeatureIndex);
            int day = PyDateTimeGET_DAY(subFeatureIndex);
            std::string dateStr = std::to_string(year) + std::to_string(month) + std::to_string(day);
            ilwFeat = this->ptr()->createSubFeature(QString::fromStdString(dateStr), geom.ptr().get()->clone());
    }
    return Feature(ilwFeat, _coverage);
}*/

QVariant Feature::attribute(const std::string& name,const QVariant& defaultValue){
    if (!defaultValue.isNull()){
        QVariant ret =  this->ptr()->cell(QString::fromStdString(name),false);
        if (!ret.isValid())
            throw std::out_of_range(QString("No attribute '%1' found").arg(name.c_str()).toStdString());
        return QVariant(ret);
    }else{
        QVariant var = this->ptr()->cell(QString::fromStdString(name), false);
        Ilwis::ColumnDefinition coldef = this->ptr()->attributedefinition(QString::fromStdString(name));
        if (coldef.isValid()){
            IlwisTypes type = coldef.datadef().domain<>()->valueType();
            if( (type & itNUMBER) || (type & itDATETIME)){
                if(var.canConvert(QVariant::Double)){
                    if(var.toDouble() == Ilwis::rUNDEF){
                        return QVariant(defaultValue);
                    }else{
                        return QVariant(var);
                    }
                }
            }else if((type & itSTRING) || (type & itDOMAINITEM)){
                if(var.canConvert(QVariant::String)){
                    if(var.toString().compare(sUNDEF) == 0){
                        return QVariant(defaultValue);
                    }else{
                        return QVariant(var);
                    }
                }
            }
        }
        throw std::out_of_range(QString("No attribute '%1' found").arg(name.c_str()).toStdString());
    }
}

qint64 Feature::attribute(const std::string& name, qint64 defaultValue){
    bool ok;
    qint64 result = this->attribute(name,QVariant(defaultValue)).toLongLong(&ok);
    if(!ok)
        throw std::invalid_argument("Could not convert " + name + " to long.");
    return result;
}

double Feature::attribute(const std::string& name, double defaultValue){
    bool ok;
    double result = this->attribute(name,QVariant(defaultValue)).toDouble(&ok);
    if(!ok)
        throw std::invalid_argument("Could not convert " + name + " to double.");
    return result;
}

std::string Feature::attribute(const std::string& name, const std::string& defaultValue){
    return this->attribute(name,QVariant(QString::fromStdString(defaultValue))).toString().toStdString();
}

void Feature::setAttribute(const std::string& name, const QVariant* value){
    this->ptr()->setCell(QString::fromStdString(name), *value);
}

void Feature::setAttribute(const std::string &name, qint64 value){
    this->ptr()->setCell(QString::fromStdString(name), QVariant(value));
}

void Feature::setAttribute(const std::string& name, double value){
    this->ptr()->setCell(QString::fromStdString(name), QVariant(value));
}

void Feature::setAttribute(const std::string& name, const std::string& value){
    this->ptr()->setCell(QString::fromStdString(name), QVariant(value.c_str()));
}

IlwisTypes Feature::ilwisType(){
    return itFEATURE;
}

Geometry* Feature::geometry(){
    return new Geometry(this);
}

void Feature::geometry(const Geometry& geom){
    this->ptr()->geometry(geom.ptr().get()->clone());
}

IlwisTypes Feature::geometryType(){
    return this->ptr()->geometryType();
}

quint64 Feature::featureId() const{
    return this->ptr()->featureid();
}

ColumnDefinition Feature::attributeDefinition(const std::string& name) const{
    Ilwis::ColumnDefinition* ilwDef = new Ilwis::ColumnDefinition(this->ptr()->attributedefinition(QString::fromStdString(name)));
    ColumnDefinition* pyDef = new ColumnDefinition(ilwDef);
    return *pyDef;
}

ColumnDefinition Feature::attributeDefinition(quint32 index) const{
    Ilwis::ColumnDefinition* ilwDef = new Ilwis::ColumnDefinition(this->ptr()->attributedefinition(index));
    ColumnDefinition* pyDef = new ColumnDefinition(ilwDef);
    return *pyDef;
}

quint32 Feature::attributeColumnCount() const{
    return this->ptr()->attributeColumnCount();
}

QVariant* Feature::checkIndex(QVariant* obj){
    if( obj == NULL)
        return new QVariant(COVERAGEATRIB); //TODO delete
    else
        return (obj);
}

void Feature::removeSubFeature(const std::string& subFeatureIndex){
    this->ptr()->removeSubFeature(QString::fromStdString(subFeatureIndex));
}

void Feature::setSubFeature(const std::string &subFeatureIndex, Feature feature){
    this->ptr()->setSubFeature(QString::fromStdString(subFeatureIndex), feature.ptr().get());
}

void Feature::removeSubFeature(double subFeatureIndex){
    this->ptr()->removeSubFeature(subFeatureIndex);
}

void Feature::setSubFeature(double subFeatureIndex, Feature feature){
    this->ptr()->setSubFeature(subFeatureIndex, feature.ptr().get());
}

quint32 Feature::subFeatureCount() const{
    return this->ptr()->subFeatureCount();
}

/*void Feature::setRecord(QVariant* pyValues, quint32 offset){ //TODO
    std::vector<QVariant> values;
    for(int i = 0; i < pyValues->Size; ++i){
        QVariant* qVar = pyValues->toList().at(i);
        values.push_back(*qVar);
    }

    this->ptr()->record(values, offset);
}*/

/*QVariant* Feature::record(){ //TODO
    Ilwis::Record ilwRec = this->ptr()->record();
    Ilwis::CRecordIter begin(ilwRec.cbegin());
    Ilwis::CRecordIter end(ilwRec.cend());

    PyObject* pyRec(newPyTuple(ilwRec.columnCount()));
    int i = 0;
    while(begin != end){
        PyObject* pyVal = QVariant2PyObject(*begin);
        setTupleItem(pyRec, i, pyVal);
        ++i;
        ++begin;
    }

    return pyRec;
}*/

VertexIterator Feature::begin(){
    Ilwis::VertexIterator ilwIt = ::begin(const_cast<std::unique_ptr<geos::geom::Geometry>&>(this->geometry()->ptr()));
    return VertexIterator(new Ilwis::VertexIterator(ilwIt));
}

VertexIterator Feature::end(){
    Ilwis::VertexIterator ilwIt = ::end(const_cast<std::unique_ptr<geos::geom::Geometry>&>(this->geometry()->ptr()));
    return VertexIterator(new Ilwis::VertexIterator(ilwIt));
}

std::shared_ptr<Ilwis::FeatureInterface> Feature::ptr() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Feature!"));
    return this->_ilwisSPFeatureI;
}


