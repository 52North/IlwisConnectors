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

Geometry::Geometry(const char *wkt): _standalone(true), _wkt(std::string(wkt)){
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

const char* Geometry::__str__(){
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

bool Geometry::fromWKT(const char* wkt){
    return this->_feature->ptr()->geometry().fromWKT(QString(wkt));
}

const char *Geometry::toWKT(){
    if(this->_standalone)
        return _wkt.c_str();
    else
        return this->ptr().toWKT().toLocal8Bit();
}

Ilwis::Geometry &Geometry::ptr() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Geometry!"));
    return this->_feature->ptr()->geometry(this->_index);
}


Coordinate::Coordinate(double x, double y, double z): _x(x), _y(y), _z(z){
    this->_2d = (this->_z == -1e308); //rUNDEF = -1e308
}

const char* pythonapi::Coordinate::__str__(){
    if (this->_2d)
        return QString("coordinate(%1,%2)").arg(this->_x).arg(this->_y).toLocal8Bit();
    else
        return QString("coordinate(%1,%2,%3)").arg(this->_x).arg(this->_y).arg(this->_z).toLocal8Bit();
}


Pixel::Pixel(qint32 x, qint32 y): _x(x), _y(y){
}

const char* Pixel::__str__(){
    return QString("pixel(%1,%2)").arg(this->_x).arg(this->_y).toLocal8Bit();
}

}//namespace pythonapi
