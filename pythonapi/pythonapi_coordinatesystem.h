#ifndef PYTHONAPI_COORDINATESYSTEM_H
#define PYTHONAPI_COORDINATESYSTEM_H

#include "pythonapi_ilwisobject.h"

namespace Ilwis{
    class CoordinateSystem;
    typedef IlwisData<CoordinateSystem> ICoordinateSystem;
}

namespace pythonapi {

    class CoordinateSystem : public pythonapi::IlwisObject{
    public:
        CoordinateSystem();
        CoordinateSystem(Ilwis::ICoordinateSystem* cs);
        CoordinateSystem(const char* resource);
        static CoordinateSystem* toCoordinateSystem(Object *obj);
    };

} // namespace pythonapi

#endif // PYTHONAPI_COORDINATESYSTEM_H
