#include <iterator>

#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericdomain.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributedefinition.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featureiterator.h"

#include "javaapi_featureiterator.h"
#include "javaapi_extension.h"
#include "javaapi_featurecoverage.h"
#include "javaapi_error.h"

using namespace javaapi;

FeatureIterator::FeatureIterator(FeatureCoverage* fc):  _coverage(fc),_ilwisFeatureIterator(new Ilwis::FeatureIterator((*fc->ptr()))){
    this->_end.reset(new Ilwis::FeatureIterator(this->ptr().end()));
}

FeatureIterator::FeatureIterator(const FeatureIterator &it): _coverage(it._coverage),_ilwisFeatureIterator(new Ilwis::FeatureIterator(it.ptr())){
    this->_end.reset(new Ilwis::FeatureIterator(this->ptr().end()));
}

Feature FeatureIterator::__next__(){
    Ilwis::FeatureIterator& iter = this->ptr();
    if (iter != (*this->_end)){
        Ilwis::SPFeatureI* f = new Ilwis::SPFeatureI(*iter);
        iter++;
        return Feature(f, this->_coverage);
    }else{
        throw StopIteration();
    }
}

bool FeatureIterator::hasNext() {
    Ilwis::FeatureIterator& iter = this->ptr();
    return (iter != (*this->_end));
}

Feature FeatureIterator::current(){
    return Feature(new Ilwis::SPFeatureI(*this->ptr()), this->_coverage);
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

//FeatureIterator FeatureIterator::end(){
//    FeatureIterator newIter = FeatureIterator(*this);
//    newIter._ilwisFeatureIterator.reset(new Ilwis::FeatureIterator(newIter.ptr().end()));
//    return newIter;
//}

Ilwis::FeatureIterator& FeatureIterator::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("use of invalid FeatureIterator (ptr)");
    return (*this->_ilwisFeatureIterator);
}
