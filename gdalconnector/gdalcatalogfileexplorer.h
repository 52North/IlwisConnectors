#ifndef GDALCATALOGFILEEXPLORER_H
#define GDALCATALOGFILEEXPLORER_H

namespace Ilwis{
namespace Gdal{
class GdalCatalogFileExplorer : public CatalogExplorer
{
public:
    GdalCatalogFileExplorer(const Ilwis::Resource &resource,const PrepareOptions& options=PrepareOptions());
    static Ilwis::CatalogExplorer *create(const Resource &resource,const PrepareOptions& options=PrepareOptions());

    bool canUse(const Resource& resource) const;

    std::vector<Ilwis::Resource> loadItems();
    QString provider() const;

    QFileInfo toLocalFile(const QUrl &datasource) const;


    NEW_CATALOGEXPLORER(GdalCatalogFileExplorer);

private:
};
}
}

#endif // GDALCATALOGFILEEXPLORER_H
