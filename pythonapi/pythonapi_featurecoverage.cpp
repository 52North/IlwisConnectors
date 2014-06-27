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
#include "pythonapi_error.h"
#include "pythonapi_pyobject.h"

using namespace pythonapi;

FeatureCoverage::FeatureCoverage(Ilwis::IFeatureCoverage *coverage):Coverage(new Ilwis::ICoverage(*coverage)){
    delete coverage;
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

IlwisTypes FeatureCoverage::featureTypes() const
{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->featureTypes();
}

unsigned int FeatureCoverage::featureCount() const{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->featureCount();
}

Feature FeatureCoverage::newFeature(Geometry &geometry){
    return Feature(this->ptr()->as<Ilwis::FeatureCoverage>()->newFeature(geometry.ptr().get()->clone()),this);
}

FeatureCoverage *FeatureCoverage::toFeatureCoverage(Object *obj){
    FeatureCoverage* ptr = dynamic_cast<FeatureCoverage*>(obj);
    if(!ptr)
        throw InvalidObject("cast to FeatureCoverage not possible");
    return ptr;
}

PyObject* FeatureCoverage::select(std::string spatialQuery){
    std::vector<quint32> vec = this->ptr()->as<Ilwis::FeatureCoverage>()->select(QString::fromStdString(spatialQuery));
    PyObject* pyTup = newPyTuple(vec.size());
    for(int i = 0; i < vec.size(); i++){
        setTupleItem(pyTup, i, PyFloatFromDouble((double)vec[i]));
    }
    return pyTup;
}


