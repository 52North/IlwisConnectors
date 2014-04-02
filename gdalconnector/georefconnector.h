#ifndef GEOREFCONNECTOR_H
#define GEOREFCONNECTOR_H

namespace Ilwis {
namespace Gdal {

class GeorefConnector : public GdalConnector
{
public:
    GeorefConnector(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
    bool loadMetaData(IlwisObject *data, const PrepareOptions &options);
    IlwisObject *create() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
};
}
}

#endif // GEOREFCONNECTOR_H
