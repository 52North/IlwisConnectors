#ifndef GDALCATALOGCONNECTOR_H
#define GDALCATALOGCONNECTOR_H

namespace Ilwis{
namespace Gdal{
class GdalCatalogConnector : public CatalogConnector
{
public:
    GdalCatalogConnector(const Ilwis::Resource &item);

    virtual bool loadItems();
    bool canUse(const QUrl& res) const;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &item, bool load=true);
protected:
    IlwisTypes _type;
};
}
}

#endif // GDALCATALOGCONNECTOR_H
