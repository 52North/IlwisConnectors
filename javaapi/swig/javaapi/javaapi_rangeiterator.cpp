#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/util/range.h"
#include "../../IlwisCore/core/ilwisobjects/domain/rangeiterator.h"
#include "../../IlwisCore/core/ilwisobjects/domain/itemrange.h"

#include <QSharedPointer>
#include "../../IlwisCore/core/kernel_global.h"

#include "javaapi_rangeiterator.h"
#include "javaapi_range.h"
#include "javaapi_error.h"
#include "javaapi_domainitem.h"

namespace javaapi{

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange>
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::RangeIterator(Range* rng)
    : _ilwisRangeIterator(new Ilwis::RangeIterator<IlwOutput, IlwRange>(rng->_range->clone())),
      _rng(rng)
{
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange>
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::RangeIterator(const RangeIterator& iter)
    : _ilwisRangeIterator(new Ilwis::RangeIterator<IlwOutput, IlwRange>(iter.ptr())),
      _rng(iter._rng)
{
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange>
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::RangeIterator(Ilwis::RangeIterator<IlwOutput, IlwRange> *iter)
    : _ilwisRangeIterator(iter)
{
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange>
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::~RangeIterator(){

}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> bool
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::__bool__() const{
    return (this->_rng && this->_rng->__bool__() && (bool)this->_ilwisRangeIterator && this->_ilwisRangeIterator->isValid());
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> std::string
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::__str__(){
    return "RangeIterator at value: " + std::to_string(current()) + " of Range: " + _rng->__str__();
}


template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>*
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::__iter__(){
    return this;
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> OutputType
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::_next(){
    Ilwis::RangeIterator<IlwOutput, IlwRange>& iter = this->ptr();
    if (iter.isValid()){
        IlwOutput t = (*iter);
        ++iter;
        return t;

//        OutputType o = t;
//        DomainItem tempDI;
//        if(TypeIsDouble<OutputType>::value)
//            return t;
//        else
//            tempDI(o);
//        return o;
    }else{
        throw StopIteration();
    }
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> bool //for java foreach, not working
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::hasNext(){
    Ilwis::RangeIterator<IlwOutput, IlwRange>& iter = this->ptr();
    _ilwisRangeIterator.get();
    _end.get();
    *iter;
    return (((*iter)!=0) && iter.isValid());
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> OutputType
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::current(){
    return *this->ptr();
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> OutputType
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::__getitem__(quint32 pos){
    return this->ptr()[pos];
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>
RangeIterator<OutputType, RangeType,IlwOutput,  IlwRange>::__radd__(int n){
    RangeIterator<OutputType, RangeType, IlwOutput, IlwRange> iter(*this);
    iter.ptr() += n;
    return iter;
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator+ (int n){
    RangeIterator<OutputType, RangeType, IlwOutput, IlwRange> iter(*this);
    iter.ptr() += n;
    return iter;
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator+= (int n){
    RangeIterator<OutputType, RangeType, IlwOutput, IlwRange> iter(*this);
    iter.ptr() += n;
    return iter;
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator- (int n){
    RangeIterator<OutputType, RangeType, IlwOutput, IlwRange> iter(*this);
    iter.ptr() -= n;
    return iter;
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator-= (int n){
    RangeIterator<OutputType, RangeType, IlwOutput, IlwRange> iter(*this);
    iter.ptr() -= n;
    return iter;
}

/*
template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> bool
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator== (const RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>& other){
    return this->ptr() == other.ptr();
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> bool
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator!= (const RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>& other){
    return this->ptr() == other.ptr();
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> bool
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator> (const RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>& other){
    return this->ptr() > other.ptr();
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> bool
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator>= (const RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>& other){
    return this->ptr() >= other.ptr();
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> bool
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator< (const RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>& other){
    return this->ptr() < other.ptr();
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> bool
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::operator<= (const RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>& other){
    return this->ptr() <= other.ptr();
}*/

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> int
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::compareTo (const RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>& other){
    if ( this->ptr() < other.ptr() )
        return -1;
    else if ( this->ptr() == other.ptr() )
        return 0;
    else
        return 1;
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> void
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::setRange (Range* rng)
{
    _rng = rng;
}

template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange> Ilwis::RangeIterator<IlwOutput, IlwRange>&
RangeIterator<OutputType, RangeType, IlwOutput, IlwRange>::ptr() const{
    //if (!this->__bool__()) //TODO error handling
    //    throw InvalidObject("use of invalid RangeIterator (ptr)");
    return (*this->_ilwisRangeIterator);
}

template class RangeIterator<double, NumericRange, double, Ilwis::NumericRange>;
//template class RangeIterator<DomainItem, ItemRange, Ilwis::SPDomainItem, Ilwis::ItemRange>;


}
