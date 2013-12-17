#ifndef PYTHONAPI_COORDINATESYSTEM_H
#define PYTHONAPI_COORDINATESYSTEM_H

#include "pythonapi_ilwisobject.h"

namespace Ilwis{
    class CoordinateSystem;
    typedef IlwisData<CoordinateSystem> ICoordinateSystem;
}

namespace pythonapi {

    class CoordinateSystem : public pythonapi::IlwisObject{
        friend class Coverage;
        friend class Engine;
    private:
        CoordinateSystem(Ilwis::ICoordinateSystem* cs);
    public:
        CoordinateSystem(const char* resource);
        static CoordinateSystem* toCoordinateSystem(Object *obj);
    };

} // namespace pythonapi

#endif // PYTHONAPI_COORDINATESYSTEM_H
