#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/geometries.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"

#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/ellipsoid.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/geodeticdatum.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/projection.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/proj4parameters.h"

#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/conventionalcoordinatesystem.h"

#include "pythonapi_coordinatesystem.h"

using namespace pythonapi;

CoordinateSystem::CoordinateSystem(Ilwis::ICoordinateSystem *cs): IlwisObject(new Ilwis::IIlwisObject(*cs)){
}

CoordinateSystem::CoordinateSystem(const char *resource){
    Ilwis::ICoordinateSystem cs;
    cs.prepare(QString(resource));
    if (cs.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(cs));
}

CoordinateSystem *CoordinateSystem::toCoordinateSystem(Object *obj){
    CoordinateSystem* ptr = dynamic_cast<CoordinateSystem*>(obj);
    if(!ptr)
        throw InvalidObject("cast to CoordinateSystem not possible");
    return ptr;
}
