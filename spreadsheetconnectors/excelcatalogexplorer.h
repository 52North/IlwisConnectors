#ifndef EXCELCATALOGEXPLORER_H
#define EXCELCATALOGEXPLORER_H

namespace Ilwis {
namespace SpreadSheetConnectors {

class ExcelCatalogExplorer : public CatalogExplorer
{
public:
    ExcelCatalogExplorer(const Ilwis::Resource &resource,const PrepareOptions& options=PrepareOptions());

    std::vector<Ilwis::Resource> loadItems();
    bool canUse(const Resource& resource) const;
    QString provider() const;

    QFileInfo toLocalFile(const QUrl &datasource) const;


    static Ilwis::CatalogExplorer *create(const Resource &resource,const PrepareOptions& options=PrepareOptions());

    NEW_CATALOGEXPLORER(ExcelCatalogExplorer);
};
}
}

#endif // EXCELCATALOGEXPLORER_H
