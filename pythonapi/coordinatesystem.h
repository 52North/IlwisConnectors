#ifndef PYTHONAPI_COORDINATESYSTEM_H
#define PYTHONAPI_COORDINATESYSTEM_H

#include "ilwisobject.h"

namespace pythonapi {

    class CoordinateSystem : public pythonapi::IlwisObject{
    public:
        CoordinateSystem();
        CoordinateSystem(const char* resource);
        static CoordinateSystem* toCoordinateSystem(Object *obj);
    };

} // namespace pythonapi

#endif // PYTHONAPI_COORDINATESYSTEM_H
