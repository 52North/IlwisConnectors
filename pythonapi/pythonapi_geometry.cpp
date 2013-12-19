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

Geometry::Geometry(std::string wkt): _standalone(true), _wkt(wkt){
}

Geometry::Geometry(Feature *feature, int index): _standalone(false),_feature(feature), _index(index){
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
        return !_wkt.empty();
    }else{
        return this->_feature != nullptr && (bool)this->_feature && this->_feature->__bool__() && this->_feature->ptr()->geometry(this->_index).isValid();
    }
}

std::string Geometry::__str__(){
    if (this->__bool__())
        return this->toWKT();
    else
        return "invalid Geometry!";
}

IlwisTypes Geometry::ilwisType(){
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Feature!"));
    return this->ptr().geometryType();
}

bool Geometry::fromWKT(const std::string& wkt){
    return this->_feature->ptr()->geometry().fromWKT(QString::fromStdString(wkt));
}

std::string Geometry::toWKT(){
    if(this->_standalone)
        return _wkt.c_str();
    else
        return this->ptr().toWKT().toStdString();
}

Ilwis::Geometry &Geometry::ptr() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Geometry!"));
    return this->_feature->ptr()->geometry(this->_index);
}

CoordinateSystem Geometry::coordinateSystem(){
}

}

Ilwis::Geometry &Geometry::ptr() const{
    else
        return this->_feature->ptr()->geometry(this->_index);
}

}//namespace pythonapi
