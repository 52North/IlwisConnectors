#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/raster.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/rastercoverage.h"

#include "pythonapi_rastercoverage.h"
#include "pythonapi_engine.h"

using namespace pythonapi;

RasterCoverage::RasterCoverage(){
    Ilwis::IRasterCoverage fc;
    fc.prepare();
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

RasterCoverage::RasterCoverage(const char *resource){
    Ilwis::IRasterCoverage fc;
    fc.prepare(QString(resource));
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

RasterCoverage* RasterCoverage::operator+(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(QString("add_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toLocal8Bit(),QString("%1 + %2").arg((*this->ptr())->name(), (*rc.ptr())->name()).toLocal8Bit());
}

RasterCoverage *RasterCoverage::operator+ (double value){
    return (RasterCoverage*)Engine::_do(QString("add_%1_%2").arg((*this->ptr())->id()).arg(value).toLocal8Bit(),QString("%1 + %2").arg((*this->ptr())->name()).arg(value).toLocal8Bit());
}

RasterCoverage *RasterCoverage::__radd__(double value){
    return (RasterCoverage*)Engine::_do(QString("add_%2_%1").arg((*this->ptr())->id()).arg(value).toLocal8Bit(),QString("%2 + %1").arg((*this->ptr())->name()).arg(value).toLocal8Bit());
}

RasterCoverage *RasterCoverage::operator-(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(QString("sub_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toLocal8Bit(),QString("%1 - %2").arg((*this->ptr())->name(), (*rc.ptr())->name()).toLocal8Bit());
}

RasterCoverage *RasterCoverage::operator-(double value){
    return (RasterCoverage*)Engine::_do(QString("sub_%1_%2").arg((*this->ptr())->id()).arg(value).toLocal8Bit(),QString("%1 - %2").arg((*this->ptr())->name()).arg(value).toLocal8Bit());
}

RasterCoverage *RasterCoverage::__rsub__(double value){
    return (RasterCoverage*)Engine::_do(QString("sub_%2_%1").arg((*this->ptr())->id()).arg(value).toLocal8Bit(),QString("%2 - %1").arg((*this->ptr())->name()).arg(value).toLocal8Bit());
}

RasterCoverage *RasterCoverage::operator*(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(QString("mul_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toLocal8Bit(),QString("%1 * %2").arg((*this->ptr())->name(), (*rc.ptr())->name()).toLocal8Bit());
}

RasterCoverage *RasterCoverage::operator*(double value){
    return (RasterCoverage*)Engine::_do(QString("mul_%1_%2").arg((*this->ptr())->id()).arg(value).toLocal8Bit(),QString("%1 * %2").arg((*this->ptr())->name()).arg(value).toLocal8Bit());
}

RasterCoverage *RasterCoverage::__rmul__(double value){
    return (RasterCoverage*)Engine::_do(QString("mul_%2_%1").arg((*this->ptr())->id()).arg(value).toLocal8Bit(),QString("%2 * %1").arg((*this->ptr())->name()).arg(value).toLocal8Bit());
}

RasterCoverage *RasterCoverage::__truediv__(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(QString("div_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toLocal8Bit(),QString("%1 / %2").arg((*this->ptr())->name(), (*rc.ptr())->name()).toLocal8Bit());
}

RasterCoverage *RasterCoverage::__truediv__(double value){
    return (RasterCoverage*)Engine::_do(QString("div_%1_%2").arg((*this->ptr())->id()).arg(value).toLocal8Bit(),QString("%1 / %2").arg((*this->ptr())->name()).arg(value).toLocal8Bit());
}

RasterCoverage *RasterCoverage::__rtruediv__(double value){
    return (RasterCoverage*)Engine::_do(QString("div_%2_%1").arg((*this->ptr())->id()).arg(value).toLocal8Bit(),QString("%2 / %1").arg((*this->ptr())->name()).arg(value).toLocal8Bit());
}

double RasterCoverage::coord2value(pythonapi::Coordinate &c){
    return this->ptr()->get<Ilwis::RasterCoverage>()->coord2value(c.data());
}

PixelIterator RasterCoverage::__iter__(){
    return PixelIterator(this);
}

double RasterCoverage::pix2value(double x, double y, double z){
    return this->ptr()->get<Ilwis::RasterCoverage>()->pix2value(Ilwis::Point3D<double>(x,y,z));
}

Size RasterCoverage::size(){
    return Size(this->ptr()->get<Ilwis::RasterCoverage>()->size());
}

void RasterCoverage::setSize(const Size &sz){
    this->ptr()->get<Ilwis::RasterCoverage>()->size(sz.data());
}

void RasterCoverage::unloadBinary(){
    this->ptr()->get<Ilwis::RasterCoverage>()->unloadBinary();
}

RasterCoverage* RasterCoverage::toRasterCoverage(Object* obj){
    RasterCoverage* ptr = static_cast<RasterCoverage*>(obj);
    if(!ptr)
        throw Ilwis::ErrorObject(QString("cast to RasterCoverage not possible"));
    return ptr;
}
