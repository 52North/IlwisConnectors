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

using namespace pythonapi;

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
