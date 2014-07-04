#ifndef PYTHONAPI_COORDINATESYSTEM_H
#define PYTHONAPI_COORDINATESYSTEM_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_util.h"

namespace Ilwis{
    class CoordinateSystem;
    typedef IlwisData<CoordinateSystem> ICoordinateSystem;
}

namespace pythonapi {

    class CoordinateSystem : public pythonapi::IlwisObject{
        friend class Coverage;
        friend class Engine;
        friend class Geometry;
        friend class FeatureCoverage;
        friend class GeoReference;
        friend class Catalog;
    private:
        CoordinateSystem(Ilwis::ICoordinateSystem* cs);
    public:
        CoordinateSystem(const std::string &resource);
        pythonapi::Envelope envelope();
        bool operator==(const CoordinateSystem& csy);
        bool operator!=(const CoordinateSystem& csy);
        static CoordinateSystem* toCoordinateSystem(Object *obj);
        std::string toWKT(quint32 spaces=0) const;
    };

} // namespace pythonapi

#endif // PYTHONAPI_COORDINATESYSTEM_H
