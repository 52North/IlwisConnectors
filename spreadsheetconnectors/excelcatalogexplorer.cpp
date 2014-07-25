#include "kernel.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "excelcatalogexplorer.h"
#include "freexl.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

REGISTER_CATALOGEXPLORER(ExcelCatalogExplorer)

ExcelCatalogExplorer::ExcelCatalogExplorer(const Ilwis::Resource &resource, const PrepareOptions &options) : CatalogExplorer(resource, options)
{

}

std::vector<Ilwis::Resource> ExcelCatalogExplorer::loadItems()
{
    QStringList sfilters;
    sfilters << "*.xls";
    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    kernel()->issues()->silent(true);
    std::vector<Resource> resources;
    for(const auto& file : files){
        QString excelfile = file.toLocalFile();
        const void *handle = 0;
        int ret = freexl_open (excelfile.toLocal8Bit(), &handle);
        if ( ret != FREEXL_OK)
            continue;
        unsigned int max_worksheet;
        ret = freexl_get_info (handle, FREEXL_BIFF_SHEET_COUNT, &max_worksheet);
        if ( ret == FREEXL_OK){
            if ( max_worksheet == 1){
                Resource res(file, itTABLE);
                resources.push_back(res);
            }else {
                Resource res(file, itCATALOG);
                resources.push_back(res);
                for (int worksheet_index = 0; worksheet_index < max_worksheet;
                     worksheet_index++)
                {
                    const char *utf8_worksheet_name;
                    freexl_get_worksheet_name (handle,worksheet_index, &utf8_worksheet_name);
                    QString urlSheet = res.url().toString() + "/" + QString(utf8_worksheet_name);
                    Resource resSheet(urlSheet, itTABLE);
                    QFileInfo inf(res.url().toLocalFile());
                    resSheet.name(inf.baseName() + "_" + QString(utf8_worksheet_name),false);
                    resources.push_back(resSheet);
                }

            }
        }
        freexl_close(handle);
    }

     mastercatalog()->addItems(resources);

     kernel()->issues()->silent(false);

     return resources;
}

bool ExcelCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    if ( QFileInfo(resource.url().toLocalFile()).isFile()) // no file can be an excel folder catalog.
        return false;
    return true;

}

QString ExcelCatalogExplorer::provider() const
{
    return "spreadsheet";
}

QFileInfo ExcelCatalogExplorer::toLocalFile(const QUrl &datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

Ilwis::CatalogExplorer *ExcelCatalogExplorer::create(const Resource &resource, const PrepareOptions &options)
{
    return new ExcelCatalogExplorer(resource, options) ;
}
