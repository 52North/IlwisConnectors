#ifndef COORDINATESYSTEMCONNECTOR_H
#define COORDINATESYSTEMCONNECTOR_H

namespace Ilwis{
namespace Gdal{

class CoordinateSystemConnector : public GdalConnector
{
public:
    CoordinateSystemConnector(const Ilwis::Resource &item, bool load=true);

    bool loadMetaData(IlwisObject *data);

    static bool canUse(const Ilwis::Resource &item);

    IlwisObject *create() const;
    static ConnectorInterface *create(const Resource &item, bool load=true);

private:
    void setProjectionParameter(OGRSpatialReferenceH handle, const char *wkt, Projection::ProjectionParamValue parmType, IProjection &projection);
};
}
}

#endif // COORDINATESYSTEMCONNECTOR_H
