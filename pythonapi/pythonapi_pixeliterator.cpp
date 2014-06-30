#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/raster.h"
#include "../../IlwisCore/core/util/tranquilizer.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/pixeliterator.h"

#include "pythonapi_pixeliterator.h"
#include "pythonapi_extension.h"
#include "pythonapi_rastercoverage.h"
#include "pythonapi_pyobject.h"
#include "pythonapi_error.h"

namespace pythonapi {

PixelIterator::PixelIterator(const PixelIterator& pi): _coverage(pi._coverage), _ilwisPixelIterator(new Ilwis::PixelIterator(pi.ptr())), _endposition(pi._endposition){
}

PixelIterator::PixelIterator(RasterCoverage *rc, const Box &b): _coverage(rc){
    if (rc && rc->__bool__())
    {
        this->_ilwisPixelIterator.reset(new Ilwis::PixelIterator((*rc->ptr()), b.data()));
        if ((bool)this->_ilwisPixelIterator && this->_ilwisPixelIterator->isValid())
            this->_endposition = this->_ilwisPixelIterator->end().linearPosition();
    }
}

PixelIterator::PixelIterator(Ilwis::PixelIterator *ilwIt){
    this->_ilwisPixelIterator.reset(ilwIt);
    if ((bool)this->_ilwisPixelIterator && this->_ilwisPixelIterator->isValid())
    {
        this->_endposition = this->_ilwisPixelIterator->end().linearPosition();
        this->_coverage = new RasterCoverage(const_cast<Ilwis::IRasterCoverage*>(&(_ilwisPixelIterator->raster())));
    }

}

PixelIterator::~PixelIterator(){
}

PixelIterator* PixelIterator::__iter__(){
    return this;
}

double PixelIterator::__next__(){
    Ilwis::PixelIterator& iter = this->ptr();
    if (iter.linearPosition() != this->_endposition){
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

std::string PixelIterator::__str__(){
    if (this->__bool__())
        return QString("PixelIterator for %1 at position %2").arg(QString::fromStdString(this->_coverage->name())).arg((QString::fromStdString(this->position().__str__()))).toStdString();
    else
        return  std::string("invalid PixelIterator");
}

double PixelIterator::__float__(){
    return (*this->ptr());
}

quint64 PixelIterator::__int__(){
    return this->ptr().linearPosition();
}

bool PixelIterator::__contains__(const Pixel &vox){
    return this->ptr().contains(vox.data());
}

Box PixelIterator::box(){
    return Box(this->ptr().box());
}

Pixel PixelIterator::position(){
    return Pixel(this->ptr().position());
}

void PixelIterator::setFlow(Flow flw){
    this->ptr().setFlow(static_cast<Ilwis::PixelIterator::Flow> (flw));
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

PixelIterator& PixelIterator::__getitem__(const Pixel &vox){
    this->ptr()[vox.data()];
    return (*this);
}

double PixelIterator::__getitem__(quint32 linearPosition){
    return this->ptr()[linearPosition];
}

void PixelIterator::__setitem__(quint32 linearPosition, double value){
    this->ptr()[linearPosition] = value;
}

PixelIterator PixelIterator::operator+ (int n){
    PixelIterator iter(*this);
    iter.ptr() += n;
    return iter;
}

PixelIterator PixelIterator::__radd__(int n){
    PixelIterator iter(*this);
    iter.ptr() += n;
    return iter;
}

PixelIterator* PixelIterator::operator+= (int n){
    this->ptr() += n;
    return this;
}

bool PixelIterator::operator== (const PixelIterator &other){
    return this->ptr() == other.ptr();
}

bool PixelIterator::operator!= (const PixelIterator &other){
    return this->ptr() != other.ptr();
}

bool PixelIterator::operator<=(const PixelIterator &other){
    return this->ptr() <= other.ptr();
}

bool PixelIterator::operator< (const PixelIterator &other){
    return this->ptr() < other.ptr();
}

bool PixelIterator::operator>=(const PixelIterator &other){
    return this->ptr() >= other.ptr();
}

bool PixelIterator::operator> (const PixelIterator &other){
    return this->ptr() > other.ptr();
}

Py_buffer* PixelIterator::asBuffer(){
    return newPyBuffer(this->ptr().operator->(),sizeof(double)*this->ptr().box().size().linearSize(), false);
}

PixelIterator PixelIterator::end(){
    PixelIterator newIter = PixelIterator(*this);
    newIter._ilwisPixelIterator.reset(new Ilwis::PixelIterator(newIter.ptr().end()));
    return newIter;
}

Ilwis::PixelIterator& PixelIterator::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("use of invalid PixelIterator (ptr)");
    return (*this->_ilwisPixelIterator);
}

} // namespace pythonapi
