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
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"

#include "featurecoverage.h"

using namespace pythonapi;

FeatureCoverage::FeatureCoverage(){
}

FeatureCoverage::FeatureCoverage(const char* resource){
    Ilwis::IFeatureCoverage fc;
    fc.prepare(QString(resource));
    this->_ilwisIIlwisObject = fc.shared_ptr();
}

FeatureCoverage::~FeatureCoverage(){
}

const char* FeatureCoverage::toStr(){
    return this->_ilwisIIlwisObject->name().toLocal8Bit();
}

unsigned int FeatureCoverage::featureCount() const{
    return std::static_pointer_cast<Ilwis::FeatureCoverage>(this->_ilwisIIlwisObject)->featureCount();
}

bool FeatureCoverage::isValid(){
    return this->_ilwisIIlwisObject->isValid();
}
