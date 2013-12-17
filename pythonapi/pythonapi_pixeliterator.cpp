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

PixelIterator::PixelIterator(const PixelIterator& pi): _coverage(pi._coverage), _ilwisPixelIterator(new Ilwis::PixelIterator(pi.ptr())), _endposition(pi._endposition){
}

PixelIterator::PixelIterator(RasterCoverage *rc, const Box &b): _coverage(rc){
    if (rc && rc->__bool__()){
        this->_ilwisPixelIterator.reset(new Ilwis::PixelIterator((*rc->ptr()), b.data()));
        if ((bool)this->_ilwisPixelIterator && this->_ilwisPixelIterator->isValid())
            this->_endposition = this->_ilwisPixelIterator->end().linearPosition();
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

const char *PixelIterator::__str__(){
    if (this->__bool__())
        return QString("PixelIterator for %1 at position %2").arg(QString(QByteArray(this->_coverage->name()))).arg((QString(QByteArray(this->position().__str__())))).toLocal8Bit();
    else
        return "invalid PixelIterator";
}

double PixelIterator::__float__(){
    return (*this->ptr());
}

quint64 PixelIterator::__int__(){
    return this->ptr().linearPosition();
}

bool PixelIterator::contains(qint32 x, qint32 y, qint32 z){
    //TODO Ilwis::PixelIterator::contains(Pixel==Point2D) is not good for 3D Iterator
    return this->ptr().contains(Ilwis::Point2D<qint32>(x,y));
}

Box PixelIterator::box(){
    return Box(this->ptr().box());
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

Ilwis::PixelIterator& PixelIterator::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("use of invalid PixelIterator (ptr)");
    return (*this->_ilwisPixelIterator);
}

} // namespace pythonapi
