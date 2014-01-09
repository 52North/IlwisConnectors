#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/angle.h"
#include "../../IlwisCore/core/util/point.h"
#include "../../IlwisCore/core/util/line.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/polygon.h"

#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/geometry.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"

#include "pythonapi_geometry.h"
#include "pythonapi_feature.h"

namespace pythonapi{

Geometry::Geometry(std::string wkt, const CoordinateSystem& cs): _standalone(true),_feature(nullptr),_index(-1), _ilwisGeometry(new Ilwis::Geometry(QString::fromStdString(wkt),cs.ptr()->get<Ilwis::CoordinateSystem>())){
}

Geometry::Geometry(Ilwis::Geometry* geometry): _standalone(true),_feature(nullptr),_index(-1),_ilwisGeometry(geometry){
}

Geometry::Geometry(Feature* feature, int index): _standalone(false),_feature(feature), _index(index), _ilwisGeometry(nullptr){
}

Geometry::~Geometry(){
}

bool Geometry::within(const Geometry &geometry) const{
    return this->ptr().within(geometry.ptr());
}

bool Geometry::contains(const Geometry &geometry) const{
    return geometry.within(*this);
}

bool Geometry::__bool__() const{
    if (this->_standalone){
        return this->_ilwisGeometry != nullptr && (bool)this->_ilwisGeometry && this->_ilwisGeometry->isValid();
    }else{
        return this->_feature != nullptr && (bool)this->_feature && this->_feature->__bool__() && this->_feature->ptr()->geometry(this->_index).isValid();
    }
}

std::string Geometry::__str__(){
    if (this->__bool__())
        return this->toWKT();
    else
        if (this->_standalone)
            return "invalid Geometry!";
        else
            return "invalid standalone Geometry!";
}

IlwisTypes Geometry::ilwisType(){
    return this->ptr().geometryType();
}

bool Geometry::fromWKT(const std::string& wkt){
    if (!this->__bool__()){
        if (this->_standalone){
            this->_standalone = true;
            this->_feature = nullptr;
            this->_index = -1;
            this->_ilwisGeometry.reset(new Ilwis::Geometry(QString::fromStdString(wkt)));
            return this->_ilwisGeometry->isValid();
        }else{
            if (this->_feature != nullptr && (bool)this->_feature && this->_feature->__bool__())
                return this->_feature->ptr()->geometry(this->_index).fromWKT(QString::fromStdString(wkt));
            else
                throw InvalidObject("invalid referenc to hosting feature of non-standalone geometry!");
        }
    }else{
        if (_standalone){
            return this->_ilwisGeometry->fromWKT(QString::fromStdString(wkt));
        }else{
            return this->_feature->ptr()->geometry(this->_index).fromWKT(QString::fromStdString(wkt));
        }
    }
}

std::string Geometry::toWKT(){
    return this->ptr().toWKT().toStdString();
}

CoordinateSystem Geometry::coordinateSystem(){
    return CoordinateSystem(new Ilwis::ICoordinateSystem(this->ptr().coordinateSystem()));
}

void Geometry::setCoordinateSystem(const CoordinateSystem &cs){
    this->ptr().coordinateSystem(cs.ptr()->get<Ilwis::CoordinateSystem>());
}

Geometry* Geometry::transform(const CoordinateSystem &cs){
    return new Geometry(new Ilwis::Geometry(this->ptr().transform(cs.ptr()->get<Ilwis::CoordinateSystem>())));
}

Envelope Geometry::envelope(){
    return Envelope(this->ptr().envelope());
}

Ilwis::Geometry& Geometry::ptr() const{
    if (this->_standalone){
        if (!this->__bool__())
            throw InvalidObject("invalid standalone Geometry!");
        return (*this->_ilwisGeometry);
    }else{
        if (!this->__bool__())
            throw InvalidObject("invalid Geometry!");
        return this->_feature->ptr()->geometry(this->_index);
    }
}

}//namespace pythonapi
