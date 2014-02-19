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

private:
    IlwisTypes extendedType(const std::multimap<QString, DataFormat> &formats, const QString &ext) const;
};
}
}

#endif // GDALCATALOGCONNECTOR_H
