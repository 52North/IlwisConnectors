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

#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"

#include "pythonapi_feature.h"
#include "pythonapi_featurecoverage.h"

#include <QVariant>

using namespace pythonapi;

Feature::Feature(std::unique_ptr<Ilwis::FeatureInterface> &ilwisFeature, FeatureCoverage* fc): _ilwisSPFeatureI(ilwisFeature), _coverage(fc){
}

bool Feature::__bool__() const{
    return (bool)this->_ilwisSPFeatureI && this->_ilwisSPFeatureI->isValid() && this->_coverage != NULL && this->_coverage->__bool__();
}

std::string Feature::__str__(){
    if (this->__bool__())
        return QString("Feature(%1)").arg(this->_ilwisSPFeatureI->featureid()).toStdString();
    else
        return QString("invalid Feature!").toStdString();
}

quint64 Feature::id(){
    return this->ptr()->featureid();
}

PyObject* Feature::__getitem__(std::string name){
    QVariant ret = this->ptr()->cell(QString::fromStdString(name),-1,false);
    if (!ret.isValid())
        throw std::out_of_range(QString("No attribute '%1' found").arg(name.c_str()).toStdString());
    return PyVariant::toPyObject(ret);
}

PyObject* Feature::__getitem__(quint32 colIndex){
    QVariant ret = this->ptr()->cell(colIndex,-1,false);
    if (!ret.isValid())
        throw std::out_of_range(QString("No attribute in '%1.' column found").arg(colIndex).toStdString());
    return PyVariant::toPyObject(ret);
}

PyObject* Feature::attribute(std::string name,const QVariant& defaultValue, int index){
    if (!defaultValue.isNull()){
        QVariant ret =  this->ptr()->cell(QString::fromStdString(name),index,false);
        if (!ret.isValid())
            throw std::out_of_range(QString("No attribute '%1' at index '%2' found").arg(name.c_str()).arg(index).toStdString());
        return PyVariant::toPyObject(ret);
    }else{
        QVariant var = this->ptr()->cell(QString::fromStdString(name),index,false);
        Ilwis::ColumnDefinition coldef = this->ptr()->columndefinition(QString::fromStdString(name));
        if (coldef.isValid()){
            IlwisTypes type = coldef.datadef().domain()->valueType();
            if( (type & itNUMBER) || (type & itDATETIME)){
                if(var.canConvert(QVariant::Double)){
                    if(var.toDouble() == Ilwis::rUNDEF){
                        return PyVariant::toPyObject(defaultValue);
                    }else{
                        return PyVariant::toPyObject(var);
                    }
                }
            }else if((type & itSTRING) || (type & itDOMAINITEM)){
                if(var.canConvert(QVariant::String)){
                    if(var.toString().compare(sUNDEF) == 0){
                        return PyVariant::toPyObject(defaultValue);
                    }else{
                        return PyVariant::toPyObject(var);
                    }
                }
            }
        }
        throw std::out_of_range(QString("No attribute '%1' at index '%2' found").arg(name.c_str()).arg(index).toStdString());
    }
}

PyObject* Feature::attribute(std::string name, qlonglong defaultValue, int index){
    return this->attribute(name,QVariant(defaultValue),index);
}

PyObject* Feature::attribute(std::string name, double defaultValue, int index){
    return this->attribute(name,QVariant(defaultValue),index);
}

PyObject* Feature::attribute(std::string name, std::string defaultValue, int index){
    return this->attribute(name,QVariant(QString::fromStdString(defaultValue)),index);
}

void Feature::__setitem__(std::string name, const PyObject* value){
    this->setAttribute(name,value);
}

void Feature::setAttribute(std::string name, const PyObject* value, int index){
    QVariant* v = PyVariant::toQVariant(value);
    this->ptr()->setCell(QString::fromStdString(name), *v , index);
    delete v;
}

void Feature::__setitem__(std::string name,qlonglong value){
    this->setAttribute(name,value);
}

void Feature::setAttribute(std::string name, qlonglong value, int index){
    this->ptr()->setCell(QString::fromStdString(name), QVariant(value), index);
}

void Feature::__setitem__(std::string name,double value){
    this->setAttribute(name,value);
}

void Feature::setAttribute(std::string name, double value, int index){
    this->ptr()->setCell(QString::fromStdString(name), QVariant(value), index);
}

void Feature::__setitem__(std::string name,std::string value){
    this->setAttribute(name,value);
}

void Feature::setAttribute(std::string name, std::string value, int index){
    this->ptr()->setCell(QString::fromStdString(name), QVariant(value.c_str()), index);
}

IlwisTypes Feature::ilwisType(){
    return itFEATURE;
}

Geometry* Feature::geometry(int index){
    return new Geometry(this, index);
}

void Feature::setGeometry(Geometry &geometry, int index){
    this->ptr()->set(geometry.ptr()->clone(), index);
}

std::unique_ptr<Ilwis::FeatureInterface> &Feature::ptr() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Feature!"));
    return this->_ilwisSPFeatureI;
}
