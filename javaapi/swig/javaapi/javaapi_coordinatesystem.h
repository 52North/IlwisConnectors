#ifndef JAVAAPI_COORDINATESYSTEM_H
#define JAVAAPI_COORDINATESYSTEM_H

#include "javaapi_ilwisobject.h"
#include "javaapi_util.h"

namespace Ilwis{
    class CoordinateSystem;
    typedef IlwisData<CoordinateSystem> ICoordinateSystem;
}

namespace javaapi {
    class CoordinateSystem : public javaapi::IlwisObject{
        friend class Coverage;
        friend class Engine;
        friend class Geometry;
        friend class FeatureCoverage;
        friend class GeoReference;
        friend class Catalog;
        friend class RasterCoverage;
    private:
        CoordinateSystem(Ilwis::ICoordinateSystem* cs);
    public:
        CoordinateSystem(const std::string &resource);
        javaapi::Envelope envelope();
        void setEnvelope(const javaapi::Envelope& env);
        bool operator==(const CoordinateSystem& csy);
        bool operator!=(const CoordinateSystem& csy);

        std::string toWKT(quint32 spaces=0) const;
        std::string toProj4() const;
        javaapi::Envelope convertEnvelope(const CoordinateSystem& pyCsy, const javaapi::Envelope& pyEnv);

        javaapi::Coordinate coord2coord(const CoordinateSystem& pyCsy, const javaapi::Coordinate& pyCoord);

        static CoordinateSystem* toCoordinateSystem(IObject *obj);
    };

} // namespace javaapi

#endif // JAVAAPI_COORDINATESYSTEM_H
