#ifndef SpreadSheetCatalogFileExplorer_H
#define SpreadSheetCatalogFileExplorer_H

namespace Ilwis {
namespace SpreadSheetConnectors {
class SpreadSheetCatalogFileExplorer : public CatalogExplorer
{
public:
    SpreadSheetCatalogFileExplorer();

    SpreadSheetCatalogFileExplorer(const Ilwis::Resource &resource,const PrepareOptions& options=PrepareOptions());
    static Ilwis::CatalogExplorer *create(const Resource &resource,const PrepareOptions& options=PrepareOptions());

    bool canUse(const Resource& resource) const;

    std::vector<Ilwis::Resource> loadItems();
    QString provider() const;

    QFileInfo toLocalFile(const QUrl &datasource) const;


    NEW_CATALOGEXPLORER(SpreadSheetCatalogFileExplorer);
};
}
}

#endif // SpreadSheetCatalogFileExplorer_H
