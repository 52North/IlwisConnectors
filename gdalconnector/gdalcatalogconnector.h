#ifndef GDALCATALOGCONNECTOR_H
#define GDALCATALOGCONNECTOR_H

namespace Ilwis{
namespace Gdal{
class GdalCatalogConnector : public CatalogConnector
{
public:
    GdalCatalogConnector(const Ilwis::Resource &resource);

    virtual bool loadItems();
    bool canUse(const Resource& resource) const;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
protected:
    IlwisTypes _type;

};
}
}

#endif // GDALCATALOGCONNECTOR_H
