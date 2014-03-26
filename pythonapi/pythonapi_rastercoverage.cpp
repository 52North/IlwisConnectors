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
#include "pythonapi_error.h"

using namespace pythonapi;



RasterCoverage::RasterCoverage(Ilwis::IRasterCoverage *coverage):Coverage(new Ilwis::ICoverage(*coverage)){
    delete coverage;
}

RasterCoverage::RasterCoverage(){
    Ilwis::IRasterCoverage fc;
    fc.prepare();
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

RasterCoverage::RasterCoverage(std::string resource){
    Ilwis::IRasterCoverage fc(QString::fromStdString(resource), itRASTER);
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

RasterCoverage* RasterCoverage::operator+(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("add_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarymathraster",
                this->__str__(),
                rc.__str__(),
                "add"
            );
}

RasterCoverage *RasterCoverage::operator+ (double value){
    return (RasterCoverage*)Engine::_do(
                QString("add_%1_%2").arg((*this->ptr())->id()).arg(value).toStdString(),
                "binarymathraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "add"
            );
}

RasterCoverage *RasterCoverage::__radd__(double value){
    return (RasterCoverage*)Engine::_do(
                QString("add_%2_%1").arg((*this->ptr())->id()).arg(value).toStdString(),
                "binarymathraster",
                QString("%1").arg(value).toStdString(),
                this->__str__(),
                "add"
            );
}

RasterCoverage *RasterCoverage::operator-(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
        QString("sub_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
        "binarymathraster",
        this->__str__(),
        rc.__str__(),
        "substract"
    );
}

RasterCoverage *RasterCoverage::operator-(double value){
    return (RasterCoverage*)Engine::_do(
                QString("sub_%1_%2").arg((*this->ptr())->id()).arg(value).toStdString(),
                "binarymathraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "substract"
            );
}

RasterCoverage *RasterCoverage::__rsub__(double value){
    return (RasterCoverage*)Engine::_do(
                QString("sub_%2_%1").arg((*this->ptr())->id()).arg(value).toStdString(),
                "binarymathraster",
                QString("%1").arg(value).toStdString(),
                this->__str__(),
                "substract"
            );
}

RasterCoverage *RasterCoverage::operator*(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
        QString("mul_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
        "binarymathraster",
        this->__str__(),
        rc.__str__(),
        "times"
    );
}

RasterCoverage *RasterCoverage::operator*(double value){
    return (RasterCoverage*)Engine::_do(
                QString("mul_%1_%2").arg((*this->ptr())->id()).arg(value).toStdString(),
                "binarymathraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "times"
            );
}

RasterCoverage *RasterCoverage::__rmul__(double value){
    return (RasterCoverage*)Engine::_do(
                QString("mul_%2_%1").arg((*this->ptr())->id()).arg(value).toStdString(),
                "binarymathraster",
                QString("%1").arg(value).toStdString(),
                this->__str__(),
                "times"
            );
}

RasterCoverage *RasterCoverage::__truediv__(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
        QString("div_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
        "binarymathraster",
        this->__str__(),
        rc.__str__(),
        "divide"
    );
}

RasterCoverage *RasterCoverage::__truediv__(double value){
    return (RasterCoverage*)Engine::_do(
                QString("div_%1_%2").arg((*this->ptr())->id()).arg(value).toStdString(),
                "binarymathraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "divide"
            );
}

RasterCoverage *RasterCoverage::__rtruediv__(double value){
    return (RasterCoverage*)Engine::_do(
                QString("div_%2_%1").arg((*this->ptr())->id()).arg(value).toStdString(),
                "binarymathraster",
                QString("%1").arg(value).toStdString(),
                this->__str__(),
                "divide"
            );
}

double RasterCoverage::coord2value(const Coordinate& c){
    return this->ptr()->get<Ilwis::RasterCoverage>()->coord2value(c.data());
}

PixelIterator RasterCoverage::__iter__(){
    return PixelIterator(this);
}

double RasterCoverage::pix2value(const PixelD &pix){
    return this->ptr()->get<Ilwis::RasterCoverage>()->pix2value(pix.data());
}

double RasterCoverage::pix2value(const Pixel &pix){
    return this->ptr()->get<Ilwis::RasterCoverage>()->pix2value(pix.data());
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
    RasterCoverage* ptr = dynamic_cast<RasterCoverage*>(obj);
    if(!ptr)
        throw InvalidObject("cast to RasterCoverage not possible");
    return ptr;
}


GeoReference RasterCoverage::geoReference(){
    return GeoReference(new Ilwis::IGeoReference(this->ptr()->get<Ilwis::RasterCoverage>()->georeference()));
}

void RasterCoverage::setGeoReference(const GeoReference& gr){
    this->ptr()->get<Ilwis::RasterCoverage>()->georeference(gr.ptr()->get<Ilwis::GeoReference>());
}
