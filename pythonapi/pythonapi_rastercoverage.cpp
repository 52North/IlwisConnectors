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
#include "pixeliterator.h"
#include "geometryhelper.h"

#include "operationoverloads.h"

#include "pythonapi_rastercoverage.h"
#include "pythonapi_engine.h"
#include "pythonapi_error.h"
#include "pythonapi_datadefinition.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_pyobject.h"
#include "pythonapi_geometry.h"

using namespace pythonapi;



RasterCoverage::RasterCoverage(Ilwis::IRasterCoverage *coverage):Coverage(new Ilwis::ICoverage(*coverage)){
//delete coverage;
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
    return this->ptr()->as<Ilwis::RasterCoverage>()->coord2value(c.data());
}

PixelIterator RasterCoverage::__iter__(){
    return PixelIterator(this);
}

double RasterCoverage::pix2value(const PixelD &pix){
    return this->ptr()->as<Ilwis::RasterCoverage>()->pix2value(pix.data());
}

double RasterCoverage::pix2value(const Pixel &pix){
    return this->ptr()->as<Ilwis::RasterCoverage>()->pix2value(pix.data());
}

Size RasterCoverage::size(){
    return Size(this->ptr()->as<Ilwis::RasterCoverage>()->size());
}

void RasterCoverage::setSize(const Size &sz){
    this->ptr()->as<Ilwis::RasterCoverage>()->size(sz.data());
}

void RasterCoverage::unloadBinary(){
    this->ptr()->as<Ilwis::RasterCoverage>()->unloadBinary();
}

RasterCoverage* RasterCoverage::toRasterCoverage(Object* obj){
    RasterCoverage* ptr = dynamic_cast<RasterCoverage*>(obj);
    if(!ptr)
        throw InvalidObject("cast to RasterCoverage not possible");
    return ptr;
}


GeoReference RasterCoverage::geoReference(){
    return GeoReference(new Ilwis::IGeoReference(this->ptr()->as<Ilwis::RasterCoverage>()->georeference()));
}

void RasterCoverage::setGeoReference(const GeoReference& gr){
    this->ptr()->as<Ilwis::RasterCoverage>()->georeference(gr.ptr()->as<Ilwis::GeoReference>());
}

const DataDefinition& RasterCoverage::datadef(quint32 layer) const{
    Ilwis::DataDefinition ilwdef = this->ptr()->as<Ilwis::RasterCoverage>()->datadef(layer);
    DataDefinition* pydef = new DataDefinition(&ilwdef);
    return *pydef;
}

DataDefinition& RasterCoverage::datadef(quint32 layer){
    Ilwis::DataDefinition ilwdef = this->ptr()->as<Ilwis::RasterCoverage>()->datadef(layer);
    DataDefinition* pydef = new DataDefinition(&ilwdef);
    return *pydef;
}

NumericStatistics* RasterCoverage::statistics(int mode){
    return new NumericStatistics(this->ptr()->as<Ilwis::RasterCoverage>()->statistics(mode));
 }

PixelIterator RasterCoverage::begin(){
    return PixelIterator(this);
}

PixelIterator RasterCoverage::end(){
    return PixelIterator(this).end();
}

PixelIterator RasterCoverage::band(PyObject* pyTrackIndex){
    std::unique_ptr<QVariant> ilwTrackIndex(PyObject2QVariant(pyTrackIndex));
    Ilwis::PixelIterator* ilwIter = new Ilwis::PixelIterator(this->ptr()->as<Ilwis::RasterCoverage>()->band(*ilwTrackIndex));
    if(ilwIter->isValid())
        return PixelIterator(ilwIter);
    else
        throw InvalidObject("Not a valid band");
}

void RasterCoverage::band(PyObject* pyTrackIndex, RasterCoverage* pyRaster){
    std::unique_ptr<QVariant> ilwTrackIndex(PyObject2QVariant(pyTrackIndex));
    PixelIterator pyIter(pyRaster);
    this->ptr()->as<Ilwis::RasterCoverage>()->band(*ilwTrackIndex, pyIter.ptr());
}

void RasterCoverage::addBand(int index, DataDefinition& pyDef, PyObject* pyTrackIndex){
    std::unique_ptr<QVariant> ilwTrackIndex(PyObject2QVariant(pyTrackIndex));
    this->ptr()->as<Ilwis::RasterCoverage>()->addBand(index, pyDef.ptr(), *ilwTrackIndex);
}

RasterCoverage RasterCoverage::select(std::string selectionQ){
    QString selectGeom = QString::fromStdString(selectionQ);
    geos::geom::Geometry *geom = Ilwis::GeometryHelper::fromWKT(selectGeom, this->ptr()->as<Ilwis::RasterCoverage>()->coordinateSystem());
    if(geom){
        Ilwis::PixelIterator iterIn(this->ptr()->as<Ilwis::RasterCoverage>(), geom);
        Ilwis::IRasterCoverage map2;
        map2.prepare();

        map2->coordinateSystem(this->ptr()->as<Ilwis::RasterCoverage>()->coordinateSystem());
        map2->georeference(this->ptr()->as<Ilwis::RasterCoverage>()->georeference());
        map2->datadef() = this->ptr()->as<Ilwis::RasterCoverage>()->datadef();

        Ilwis::PixelIterator iterOut(map2, geom);

        Ilwis::PixelIterator iterEnd = this->ptr()->as<Ilwis::RasterCoverage>()->end();
        while( iterIn != iterEnd){
            *iterOut = *iterIn;
            ++iterOut;
            ++iterIn;
        }

        return RasterCoverage(&map2);
    }
    else{
        throw InvalidObject("Not a valid geometry description");
    }

    delete geom;
}

RasterCoverage RasterCoverage::select(Geometry& geom){
    return select(geom.toWKT());
}

