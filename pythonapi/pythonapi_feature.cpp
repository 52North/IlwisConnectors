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
    return !this->_ilwisSPFeatureI->isNull() && this->_coverage != NULL && this->_coverage->__bool__() && this->_ilwisSPFeatureI->data()->isValid();
}

const char* Feature::__str__(){
    if (this->__bool__())
        return QString("Feature(%1)").arg(this->_ilwisSPFeatureI->data()->featureid()).toLocal8Bit();
    else
        return QString("invalid Feature!").toLocal8Bit();
}

PyVariant *Feature::attribute(const char *name, int index){
    return new PyVariant(new QVariant(this->ptr()(QString(name),index)));
}

void Feature::attribute(const char *name, PyVariant &value, int index){
    QVariant* tmp = value.clone();
    this->ptr()->cell(QString(name), (*tmp), index);
    delete tmp;
}

IlwisTypes Feature::ilwisType(){
    return itFEATURE;
}

Ilwis::SPFeatureI Feature::ptr() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Feature!"));
    return (*this->_ilwisSPFeatureI);
}
