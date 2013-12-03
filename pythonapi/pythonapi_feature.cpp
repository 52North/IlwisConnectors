#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/util/angle.h"
#include "../../IlwisCore/core/util/point.h"
#include "../../IlwisCore/core/util/line.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/polygon.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/geometry.h"

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

Feature::Feature(Ilwis::SPFeatureI* ilwisFeature, FeatureCoverage* fc): _ilwisSPFeatureI(ilwisFeature), _coverage(fc){
}

bool Feature::__bool__() const{
    return !this->_ilwisSPFeatureI->isNull() && this->_coverage != NULL && this->_coverage->__bool__();// && this->_ilwisSPFeatureI->data()->isValid();
}

const char* Feature::__str__(){
    if (this->__bool__())
        return QString("Feature(%1)").arg(this->_ilwisSPFeatureI->data()->featureid()).toLocal8Bit();
    else
        return QString("invalid Feature!").toLocal8Bit();
}

quint64 Feature::id(){
    return this->ptr()->featureid();
}

PyVariant *Feature::attribute(const char *name, int index){
    return new PyVariant(new QVariant(this->ptr()(QString(name),index,false)));
}

PyVariant *Feature::attribute(const char *name, PyVariant &defaultValue, int index){
    QVariant* var = new QVariant(this->ptr()(QString(name),index,false));
    Ilwis::ColumnDefinition coldef = this->ptr()->columndefinition(QString(name));
    if (coldef.isValid()){
        if( coldef.datadef().domain()->ilwisType() & itNUMERICDOMAIN){
            if(var->canConvert(QVariant::Double)){
                if(var->toDouble() == Ilwis::rUNDEF){
                    return new PyVariant(defaultValue);
                }else{
                    return new PyVariant(var);
                }
            }
        }else if((coldef.datadef().domain()->ilwisType() & itTEXTDOMAIN) || (coldef.datadef().domain()->ilwisType() & itITEMDOMAIN)){
            if(var->canConvert(QVariant::String)){
                if(var->toString().compare(sUNDEF) == 0){
                    return new PyVariant(defaultValue);
                }else{
                    return new PyVariant(var);
                }
            }
        }
    }
    throw Ilwis::ErrorObject(QString("invalid value (maybe wrong column name '%1' due to case sensitivity?)").arg(name));
}

void Feature::setAttribute(const char *name, PyVariant &value, int index){
    QVariant* tmp = value.clone();
    this->ptr()->setCell(QString(name), (*tmp), index);
    delete tmp;
}

void Feature::setAttribute(const char *name, int value, int index){
    this->ptr()->setCell(QString(name), QVariant(value), index);
}

void Feature::setAttribute(const char *name, uint value, int index){
    this->ptr()->setCell(QString(name), QVariant(value), index);
}

void Feature::setAttribute(const char *name, qlonglong value, int index){
    this->ptr()->setCell(QString(name), QVariant(value), index);
}

void Feature::setAttribute(const char *name, qulonglong value, int index){
    this->ptr()->setCell(QString(name), QVariant(value), index);
}

void Feature::setAttribute(const char *name, bool value, int index){
    this->ptr()->setCell(QString(name), QVariant(value), index);
}

void Feature::setAttribute(const char *name, double value, int index){
    this->ptr()->setCell(QString(name), QVariant(value), index);
}

void Feature::setAttribute(const char *name, float value, int index){
    this->ptr()->setCell(QString(name), QVariant(value), index);
}

void Feature::setAttribute(const char *name, const char *value, int index){
    this->ptr()->setCell(QString(name), QVariant(value), index);
}

IlwisTypes Feature::ilwisType(){
    return itFEATURE;
}

Geometry* Feature::geometry(int index){
    return new Geometry(this, index);
}

void Feature::setGeometry(Geometry &geometry, int index){
    this->ptr()->set(geometry.ptr());
}

Ilwis::SPFeatureI Feature::ptr() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Feature!"));
    return (*this->_ilwisSPFeatureI);
}
