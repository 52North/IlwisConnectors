#include <iterator>

#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericdomain.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featureiterator.h"

#include "pythonapi_featureiterator.h"
#include "pythonapi_ilwis.h"
#include "pythonapi_featurecoverage.h"

using namespace pythonapi;

FeatureIterator::FeatureIterator(FeatureCoverage* fc):  _coverage(fc),_ilwisFeatureIterator(new Ilwis::FeatureIterator((*fc->ptr()))){
    this->_end.reset(new Ilwis::FeatureIterator(this->ptr().end()));
}

FeatureIterator::FeatureIterator(const FeatureIterator &it): _coverage(it._coverage),_ilwisFeatureIterator(new Ilwis::FeatureIterator(it.ptr())){
    this->_end.reset(new Ilwis::FeatureIterator(this->ptr().end()));
}

Feature FeatureIterator::__next__(){
    Ilwis::FeatureIterator& iter = this->ptr();
    if (iter != (*this->_end)){
        std::unique_ptr<Ilwis::FeatureInterface>& f = (*iter);
        iter++;
        return Feature(f, this->_coverage);
    }else{
        throw StopIteration();
    }
}

Feature FeatureIterator::current(){
    return Feature((*this->ptr()),this->_coverage);
}

FeatureIterator *FeatureIterator::__iter__(){
    return this;
}

std::string FeatureIterator::__str__(){
    if(this->__bool__()){
        return std::string("FeatureIterator for ")+this->_coverage->name();
    }else{
        return "invalid FeatureIterator!";
    }
}

bool FeatureIterator::operator==(const FeatureIterator &other){
    return this->ptr() == other.ptr();
}

bool FeatureIterator::operator!=(const FeatureIterator &other){
    return this->ptr() != other.ptr();
}

FeatureIterator FeatureIterator::operator+(int n){
    FeatureIterator temp(*this);
    temp.ptr() + n;
    return temp;
}

FeatureIterator FeatureIterator::__radd__(int n){
    return this->operator +(n);
}

FeatureIterator FeatureIterator::operator-(int n){
    FeatureIterator temp(*this);
    temp.ptr() - n;
    return temp;
}

bool FeatureIterator::__bool__() const{
    return (this->_coverage && this->_coverage->__bool__() && (bool)this->_ilwisFeatureIterator);
}

Ilwis::FeatureIterator& FeatureIterator::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("use of invalid FeatureIterator (ptr)");
    return (*this->_ilwisFeatureIterator);
}
