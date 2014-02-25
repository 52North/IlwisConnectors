#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/util/angle.h"

#include "../../IlwisCore/core/util/geometries.h"
#include "../../IlwisCore/core/util/box.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"

#include "geos/geom/Geometry.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"

#include "pythonapi_featurecoverage.h"

using namespace pythonapi;

FeatureCoverage::FeatureCoverage(Ilwis::IFeatureCoverage *coverage):Coverage(new Ilwis::ICoverage(*coverage)){
}

FeatureCoverage::FeatureCoverage(){
    Ilwis::IFeatureCoverage fc;
    fc.prepare();
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

FeatureCoverage::FeatureCoverage(std::string resource){
    Ilwis::IFeatureCoverage fc(QString::fromStdString(resource), itFEATURE);
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

FeatureIterator FeatureCoverage::__iter__(){
    return FeatureIterator(this);
}

unsigned int FeatureCoverage::featureCount() const{
    return this->ptr()->get<Ilwis::FeatureCoverage>()->featureCount();
}

Feature FeatureCoverage::newFeature(Geometry &geometry){
    return Feature(this->ptr()->get<Ilwis::FeatureCoverage>()->newFeature(geometry.ptr().get()->clone()),this);
}

FeatureCoverage *FeatureCoverage::toFeatureCoverage(Object *obj){
    FeatureCoverage* ptr = dynamic_cast<FeatureCoverage*>(obj);
    if(!ptr)
        throw InvalidObject("cast to FeatureCoverage not possible");
    return ptr;
}



