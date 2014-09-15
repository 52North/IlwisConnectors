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
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"

#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/geometryhelper.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/vertexiterator.h"

#include "pythonapi_feature.h"
#include "pythonapi_featurecoverage.h"
#include "pythonapi_columndefinition.h"

#include "pythonapi_qvariant.h"
#include "pythonapi_vertexiterator.h"
#include <QVariant>



using namespace pythonapi;

Feature::Feature(std::unique_ptr<Ilwis::FeatureInterface> &ilwisFeature, FeatureCoverage* fc): _ilwisSPFeatureI(ilwisFeature), _coverage(fc){
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

quint64 Feature::id(){
    return this->ptr()->featureid();
}

PyObject* Feature::__getitem__(std::string name){
    QVariant ret = this->ptr()->cell(QString::fromStdString(name),COVERAGEATRIB,false);
    if (!ret.isValid())
        throw std::out_of_range(QString("No attribute '%1' found").arg(name.c_str()).toStdString());
    return QVariant2PyObject(ret);
}

PyObject* Feature::__getitem__(quint32 colIndex){
    QVariant ret = this->ptr()->cell(colIndex,COVERAGEATRIB,false);
    if (!ret.isValid())
        throw std::out_of_range(QString("No attribute in '%1.' column found").arg(colIndex).toStdString());
    return QVariant2PyObject(ret);
}

PyObject* Feature::attribute(std::string name,const QVariant& defaultValue, const QVariant& index){
    if (!defaultValue.isNull()){
        QVariant ret =  this->ptr()->cell(QString::fromStdString(name),index,false);
        if (!ret.isValid())
            throw std::out_of_range(QString("No attribute '%1' at index '%2' found").arg(name.c_str()).arg(index.toString()).toStdString());
        return QVariant2PyObject(ret);
    }else{
        QVariant var = this->ptr()->cell(QString::fromStdString(name),index,false);
        Ilwis::ColumnDefinition coldef = this->ptr()->columndefinition(QString::fromStdString(name));
        if (coldef.isValid()){
            IlwisTypes type = coldef.datadef().domain<>()->valueType();
            if( (type & itNUMBER) || (type & itDATETIME)){
                if(var.canConvert(QVariant::Double)){
                    if(var.toDouble() == Ilwis::rUNDEF){
                        return QVariant2PyObject(defaultValue);
                    }else{
                        return QVariant2PyObject(var);
                    }
                }
            }else if((type & itSTRING) || (type & itDOMAINITEM)){
                if(var.canConvert(QVariant::String)){
                    if(var.toString().compare(sUNDEF) == 0){
                        return QVariant2PyObject(defaultValue);
                    }else{
                        return QVariant2PyObject(var);
                    }
                }
            }
        }
        throw std::out_of_range(QString("No attribute '%1' at index '%2' found").arg(name.c_str()).arg(index.toString()).toStdString());
    }
}

PyObject* Feature::attribute(std::string name, qint64 defaultValue, PyObject* index){
    QVariant* ilwTrackIndex = checkIndex(index);
    return this->attribute(name,QVariant(defaultValue),*ilwTrackIndex);
}

PyObject* Feature::attribute(std::string name, double defaultValue, PyObject* index){
    QVariant* ilwTrackIndex = checkIndex(index);
    return this->attribute(name,QVariant(defaultValue),*ilwTrackIndex);
}

PyObject* Feature::attribute(std::string name, std::string defaultValue, PyObject* index){
    QVariant* ilwTrackIndex = checkIndex(index);
    return this->attribute(name,QVariant(QString::fromStdString(defaultValue)),*ilwTrackIndex);
}

void Feature::__setitem__(std::string name, const PyObject* value){
    this->setAttribute(name,value);
}

void Feature::setAttribute(std::string name, const PyObject* value, PyObject* index){
    QVariant* ilwTrackIndex = checkIndex(index);
    QVariant* v = PyObject2QVariant(value);
    this->ptr()->setCell(QString::fromStdString(name), *v , *ilwTrackIndex);
    delete v;
}

void Feature::__setitem__(std::string name, qint64 value){
    this->setAttribute(name,value);
}

void Feature::setAttribute(std::string name, qint64 value, PyObject* index){
    QVariant* ilwTrackIndex = checkIndex(index);
    this->ptr()->setCell(QString::fromStdString(name), QVariant(value), *ilwTrackIndex);
}

void Feature::__setitem__(std::string name,double value){
    this->setAttribute(name,value);
}

void Feature::setAttribute(std::string name, double value, PyObject* index){
    QVariant* ilwTrackIndex = checkIndex(index);
    this->ptr()->setCell(QString::fromStdString(name), QVariant(value), *ilwTrackIndex);
}

void Feature::__setitem__(std::string name,std::string value){
    this->setAttribute(name,value);
}

void Feature::setAttribute(std::string name, std::string value, PyObject* index){
    QVariant* ilwTrackIndex = checkIndex(index);
    this->ptr()->setCell(QString::fromStdString(name), QVariant(value.c_str()), *ilwTrackIndex);
}

IlwisTypes Feature::ilwisType(){
    return itFEATURE;
}

Geometry* Feature::geometry(PyObject* index){
    if( PyIsNone(index) || index == NULL)
        index = PyLongFromLongLong(COVERAGEATRIB);
    return new Geometry(this, index);
}

void Feature::setGeometry(Geometry &geometry, PyObject* index){
    QVariant* ilwTrackIndex = checkIndex(index);
    this->ptr()->set(geometry.ptr()->clone(), *ilwTrackIndex);
}

void Feature::addGeometry(Geometry &geometry, PyObject* index){
    QVariant* ilwTrackIndex = checkIndex(index);
    this->ptr()->add(geometry.ptr()->clone(), *ilwTrackIndex);
}

void Feature::removeGeometry(PyObject *index){
    QVariant* ilwTrackIndex = checkIndex(index);
    this->ptr()->remove(*ilwTrackIndex);
}

quint64 Feature::featureId() const{
    return this->ptr()->featureid();
}

quint32 Feature::trackSize() const{
    return this->ptr()->trackSize();
}

ColumnDefinition Feature::columnDefinition(const std::string& name, bool coverages) const{
    Ilwis::ColumnDefinition* ilwDef = new Ilwis::ColumnDefinition(this->ptr()->columndefinition(QString::fromStdString(name), coverages));
    ColumnDefinition* pyDef = new ColumnDefinition(ilwDef);
    return *pyDef;
}

ColumnDefinition Feature::columnDefinition(quint32 index, bool coverages) const{
    Ilwis::ColumnDefinition* ilwDef = new Ilwis::ColumnDefinition(this->ptr()->columndefinition(index, coverages));
    ColumnDefinition* pyDef = new ColumnDefinition(ilwDef);
    return *pyDef;
}

PyObject* Feature::trackIndexValue(quint32 index){
    QVariant qvar = this->ptr()->trackIndexValue(index);
    return QVariant2PyObject(qvar);
}

QVariant* Feature::checkIndex(PyObject *obj){
    if( PyIsNone(obj) || obj == NULL)
        return new QVariant(COVERAGEATRIB);
    else
        return PyObject2QVariant(obj);
}

VertexIterator Feature::begin(){
    Ilwis::VertexIterator ilwIt = ::begin(const_cast<std::unique_ptr<geos::geom::Geometry>&>(this->geometry()->ptr()));
    return VertexIterator(new Ilwis::VertexIterator(ilwIt));
}

VertexIterator Feature::end(){
    Ilwis::VertexIterator ilwIt = ::end(const_cast<std::unique_ptr<geos::geom::Geometry>&>(this->geometry()->ptr()));
    return VertexIterator(new Ilwis::VertexIterator(ilwIt));
}

std::unique_ptr<Ilwis::FeatureInterface> &Feature::ptr() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Feature!"));
    return this->_ilwisSPFeatureI;
}


