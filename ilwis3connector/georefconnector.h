#ifndef GEOREFCONNECTOR_H
#define GEOREFCONNECTOR_H

namespace Ilwis {
namespace Ilwis3 {
class GeorefConnector : public Ilwis3Connector
{
public:
    GeorefConnector(const Ilwis::Resource &resource, bool load=true);
    bool loadMetaData(IlwisObject *data);
    bool storeMetaData(IlwisObject *obj);
    IlwisObject *create() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);

private:
    bool loadGeoref(const IniFile &odf, IlwisObject *data);
    IlwisObject *createGeoreference(const IniFile &odf) const;
    bool loadGeorefCorners(const IniFile &odf, Ilwis::IlwisObject *data);
    bool loadGeorefTiepoints(const IniFile &odf, GeoReference *grf);
};
}
}

#endif // GEOREFCONNECTOR_H
