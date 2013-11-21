#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/angle.h"
#include "../../IlwisCore/core/util/point.h"
#include "../../IlwisCore/core/util/line.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/polygon.h"

#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/geometry.h"

#include "pythonapi_geometry.h"

namespace pythonapi{

Geometry::Geometry():_ilwisGeometry(nullptr){
}

Geometry::Geometry(Ilwis::Geometry* geometry):_ilwisGeometry(geometry){
}

Geometry::~Geometry(){
}

bool Geometry::within(const Geometry &geometry) const{
    return this->_ilwisGeometry->within((*geometry._ilwisGeometry));
}

bool Geometry::contains(const Geometry &geometry) const{
    return geometry.within(*this);
}

bool Geometry::__bool__() const{
    return (bool)this->_ilwisGeometry && this->_ilwisGeometry->isValid();
}

const char* Geometry::__str__(){
    if (this->__bool__())
        return "Geometry";
    else
        return "invalid Geometry!";
}

IlwisTypes Geometry::ilwisType(){
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Feature!"));
    return this->_ilwisGeometry->ilwisType();
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
