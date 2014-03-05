#ifndef COORDINATESYSTEMCONNECTOR_H
#define COORDINATESYSTEMCONNECTOR_H

namespace Ilwis {
namespace Ilwis3{
class CoordinateSystemConnector : public Ilwis3Connector
{
public:
    CoordinateSystemConnector(const Resource &resource, bool load=true);
    bool loadMetaData(IlwisObject *data);
    bool storeMetaData(IlwisObject *data);

    static bool canUse(const Ilwis::Resource &resource, const UPCatalogConnector &container);

    IlwisObject *create() const;
    static ConnectorInterface *create(const Resource &resource, bool load=true);

private:
    GeodeticDatum *getDatum(QString& ellipsoid);
    IProjection getProjection(Ilwis::ConventionalCoordinateSystem *csycc);
    IEllipsoid getEllipsoid();

    static QString prjParam2IlwisName(Projection::ProjectionParamValue parm);
};
}
}

#endif // COORDINATESYSTEMCONNECTOR_H
