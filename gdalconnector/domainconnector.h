#ifndef DOMAINCONNECTOR_H
#define DOMAINCONNECTOR_H

namespace Ilwis{
namespace Gdal{
class DomainConnector : public GdalConnector
{
public:
    DomainConnector(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());

    bool loadMetaData(IlwisObject *data, const PrepareOptions &options);

    static ConnectorInterface *create(const Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
    IlwisObject *create() const;
private:
    bool handleThematicDomains(IlwisObject *);
    bool handleValueDomains(IlwisObject *data);

};
}
}

#endif // DOMAINCONNECTOR_H
