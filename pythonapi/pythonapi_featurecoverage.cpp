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
#include "geos/geom/CoordinateFilter.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featureiterator.h"
#include "csytransform.h"
#include "geometryhelper.h"

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

FeatureCoverage::FeatureCoverage(const std::string& resource){
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

void FeatureCoverage::featureTypes(IlwisTypes type)
{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->featureTypes(type);
}

unsigned int FeatureCoverage::featureCount() const{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->featureCount();
}

void FeatureCoverage::setFeatureCount(IlwisTypes type, quint32 geomCnt, quint32 subGeomCnt, int index){
    this->ptr()->as<Ilwis::FeatureCoverage>()->setFeatureCount(type, geomCnt, subGeomCnt, index);
}

Feature FeatureCoverage::newFeature(const std::string& wkt, const CoordinateSystem& csy, bool load){
    return Feature(this->ptr()->as<Ilwis::FeatureCoverage>()->newFeature(QString::fromStdString(wkt), csy.ptr()->as<Ilwis::CoordinateSystem>(), load),this);
}

Feature FeatureCoverage::newFeature(const Geometry &geometry){
    return Feature(this->ptr()->as<Ilwis::FeatureCoverage>()->newFeature(geometry.ptr().get()->clone()),this);
}

Feature FeatureCoverage::newFeatureFrom(const Feature& feat, const CoordinateSystem& csy){
    return Feature(this->ptr()->as<Ilwis::FeatureCoverage>()->newFeatureFrom(feat.ptr().get()->clone(), csy.ptr()->as<Ilwis::CoordinateSystem>()),this);
}

FeatureCoverage *FeatureCoverage::toFeatureCoverage(Object *obj){
    FeatureCoverage* ptr = dynamic_cast<FeatureCoverage*>(obj);
    if(!ptr)
        throw InvalidObject("cast to FeatureCoverage not possible");
    return ptr;
}

PyObject* FeatureCoverage::select(const std::string& spatialQuery){
    std::vector<quint32> vec = this->ptr()->as<Ilwis::FeatureCoverage>()->select(QString::fromStdString(spatialQuery));
    PyObject* pyTup = newPyTuple(vec.size());
    for(int i = 0; i < vec.size(); i++){
        setTupleItem(pyTup, i, PyLongFromUnsignedLongLong(vec[i]));
    }
    return pyTup;
}

void FeatureCoverage::reprojectFeatures(const CoordinateSystem& csy){
    Ilwis::IFeatureCoverage fc = this->ptr()->as<Ilwis::FeatureCoverage>();
    Ilwis::ICoordinateSystem ilwCsy = csy.ptr()->as<Ilwis::CoordinateSystem>();
    for(const auto &feat : fc ){
        for(int i=0; i < feat->trackSize(); ++i){
              Ilwis::UPGeometry& geom = feat->geometry(i);
              if ( ilwCsy.isValid() && !ilwCsy->isEqual(coordinateSystem().ptr()->as<Ilwis::CoordinateSystem>().ptr())){
                  Ilwis::CsyTransform trans(coordinateSystem().ptr()->as<Ilwis::CoordinateSystem>(), ilwCsy);
                  geom->apply_rw(&trans);
                  geom->geometryChangedAction();
              }
              Ilwis::GeometryHelper::setCoordinateSystem(geom.get(), ilwCsy.ptr());
        }
    }
    Ilwis::ICoordinateSystem oldCsy = coordinateSystem().ptr()->as<Ilwis::CoordinateSystem>();
    fc->coordinateSystem(ilwCsy);
    Ilwis::Envelope newEnv = oldCsy->convertEnvelope(ilwCsy, fc->envelope());
    fc->envelope(newEnv);
}

quint32 FeatureCoverage::maxIndex() const{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->maxIndex();
}

FeatureCoverage *FeatureCoverage::clone(){
    Ilwis::IFeatureCoverage ilwFc = this->ptr()->as<Ilwis::FeatureCoverage>()->clone();
    return new FeatureCoverage(&ilwFc);
}

IlwisTypes FeatureCoverage::geometryType(const Geometry& geom){
    return this->ptr()->as<Ilwis::FeatureCoverage>()->geometryType(geom.ptr().get());
}
