#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/raster.h"
#include "../../IlwisCore/core/util/linerasterizer.h"
#include "../../IlwisCore/core/util/tranquilizer.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/pixeliterator.h"

#include "pythonapi_pixeliterator.h"
#include "pythonapi_ilwis.h"
#include "pythonapi_rastercoverage.h"

namespace pythonapi {

PixelIterator::PixelIterator(const PixelIterator& pi): _coverage(pi._coverage), _ilwisPixelIterator(new Ilwis::PixelIterator(pi.ptr())){
}

PixelIterator::PixelIterator(RasterCoverage *rc): _coverage(rc){
    if (rc && rc->__bool__())
        this->_ilwisPixelIterator.reset(new Ilwis::PixelIterator((*rc->ptr())));
}

PixelIterator* PixelIterator::__iter__(){
    return this;
}

double PixelIterator::__next__(){
    Ilwis::PixelIterator& iter = this->ptr();
    if (iter != iter.end()){
        double d = (*iter);
        ++iter;
        return d;
    }else{
        throw StopIteration();
    }
}

bool PixelIterator::__bool__() const{
    return (this->_coverage && this->_coverage->__bool__() && (bool)this->_ilwisPixelIterator && this->_ilwisPixelIterator->isValid());
}

const char *PixelIterator::__str__(){
    if (this->__bool__())
        return QString("PixelIterator for %1").arg(this->_coverage->__str__()).toLocal8Bit();
    else
        return "invalid PixelIterator";
}

double PixelIterator::__float__(){
    return (*this->ptr());
}

quint64 PixelIterator::__int__(){
    return this->ptr().linearPosition();
}

Voxel PixelIterator::position(){
    return Voxel(this->ptr().position());
}

bool PixelIterator::xChanged(){
    return this->ptr().xchanged();
}

bool PixelIterator::yChanged(){
    return this->ptr().ychanged();
}

bool PixelIterator::zChanged(){
    return this->ptr().zchanged();
}

PixelIterator& PixelIterator::__getitem__(Voxel &vox){
    this->ptr()[vox.data()];
    return (*this);
}

double PixelIterator::__getitem__(quint32 position){
    return this->ptr()[position];
}

void PixelIterator::__setitem__(quint32 position, double value){
    this->ptr()[position] = value;
}

PixelIterator PixelIterator::__add__(int n){
    PixelIterator iter(*this);
    iter.ptr() + n;
    return iter;
}

PixelIterator PixelIterator::__radd__(int n){
    PixelIterator iter(*this);
    iter.ptr() + n;
    return iter;
}

PixelIterator* PixelIterator::__iadd__(int n){
    this->ptr() += n;
    return this;
}

bool PixelIterator::__eq__(const PixelIterator &other){
    return this->ptr() == other.ptr();
}

bool PixelIterator::__ne__(const PixelIterator &other){
    return this->ptr() != other.ptr();
}

bool PixelIterator::__le__(const PixelIterator &other){
    return this->ptr() <= other.ptr();
}

bool PixelIterator::__lt__(const PixelIterator &other){
    return this->ptr() < other.ptr();
}

bool PixelIterator::__ge__(const PixelIterator &other){
    return this->ptr() >= other.ptr();
}

bool PixelIterator::__gt__(const PixelIterator &other){
    return this->ptr() > other.ptr();
}

Ilwis::PixelIterator& PixelIterator::ptr() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject("use of invalid PixelIterator (ptr)");
    return (*this->_ilwisPixelIterator);
}

} // namespace pythonapi
