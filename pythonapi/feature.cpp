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

#include "feature.h"

#include <QVariant>

using namespace pythonapi;

Feature::Feature(Ilwis::SPFeatureI* ilwisFeature): _ilwisSPFeatureI(ilwisFeature){
}

bool Feature::__bool__() const{
    return !this->_ilwisSPFeatureI->isNull() && this->_ilwisSPFeatureI->data()->isValid();
}

const char* Feature::__str__(){
    if (this->__bool__())
        return QString("IlwisFeature(%1)").arg(this->_ilwisSPFeatureI->data()->featureid()).toLocal8Bit();
    else
        return QString("invalid IlwisFeature!").toLocal8Bit();
}

PyVariant *Feature::cell(const char *name, int index){
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("cannot call cell on invalid Feature"));
    return new PyVariant(new QVariant((*this->_ilwisSPFeatureI)(QString(name),index)));
}