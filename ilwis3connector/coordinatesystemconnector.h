#ifndef COORDINATESYSTEMCONNECTOR_H
#define COORDINATESYSTEMCONNECTOR_H

namespace Ilwis {
namespace Ilwis3{
class CoordinateSystemConnector : public Ilwis3Connector
{
public:
    CoordinateSystemConnector(const Resource &resource, bool load=true);
    bool loadMetaData(IlwisObject *data);

    static bool canUse(const Ilwis::Resource &resource, const UPContainerConnector &container);

    IlwisObject *create() const;
    static ConnectorInterface *create(const Resource &resource, bool load=true);

    static QString createCsyFromCode(const QString &code);
private:
    GeodeticDatum *getDatum(QString& ellipsoid);
    IProjection getProjection();
    IEllipsoid getEllipsoid();
};
}
}

#endif // COORDINATESYSTEMCONNECTOR_H
