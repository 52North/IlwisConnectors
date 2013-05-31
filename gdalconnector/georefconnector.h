#ifndef GEOREFCONNECTOR_H
#define GEOREFCONNECTOR_H

namespace Ilwis {
namespace Gdal {

class GeorefConnector : public GdalConnector
{
public:
    GeorefConnector(const Ilwis::Resource &item, bool load=true);
    bool loadMetaData(IlwisObject *data);
    IlwisObject *create() const;

    static ConnectorInterface *create(const Ilwis::Resource &item, bool load=true);
};
}
}

#endif // GEOREFCONNECTOR_H
