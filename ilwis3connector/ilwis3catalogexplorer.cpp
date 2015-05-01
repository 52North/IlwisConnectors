#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "odfitem.h"
#include "ilwiscontext.h"
#include "dataformat.h"
#include "tranquilizer.h"
#include "foldercatalogexplorer.h"
#include "ilwis3catalogexplorer.h"

using namespace Ilwis;
using namespace Ilwis3;

REGISTER_CATALOGEXPLORER(Ilwis3CatalogExplorer)

CatalogExplorer *Ilwis3CatalogExplorer::create(const Resource &resource,const IOOptions& options=IOOptions()) {
        return new Ilwis3CatalogExplorer(resource, options);
}

Ilwis3CatalogExplorer::Ilwis3CatalogExplorer(const Resource &resource, const IOOptions &options) : FolderCatalogExplorer(resource, true, options)
{
}

inline uint qHash(const QFileInfo& inf ){
    return ::qHash(inf.canonicalFilePath());
}

std::vector<Resource> Ilwis3CatalogExplorer::loadItems(const IOOptions &)
{
    QVariantList filters = DataFormat::getFormatProperties(DataFormat::fpEXTENSION, itILWISOBJECT,"ilwis3");
    QStringList sfilters;
    for(QVariant& ext : filters)
        sfilters += "*." + ext.toString();
    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    std::set<ODFItem> odfitems;
    QHash<QString, quint64> names;
    UPTranquilizer trq(Tranquilizer::create(context()->runMode()));
    trq->prepare("ilwis3 connector",source().toLocalFile(),files.size());
    kernel()->issues()->silent(true);  // error messages during scan are not needed
    try{
        foreach(const QUrl& url, files) {
            QFileInfo file = toLocalFile(url);
            IlwisTypes tp = Ilwis3Connector::ilwisType(file.fileName());
            if ( tp & itILWISOBJECT ) {
                ODFItem item(file);
                odfitems.insert(item);
                names[url.toString().toLower()] = item.id();
            }
            if (!trq->update(1))
                return std::vector<Resource>();
        }

        std::vector<ODFItem> items;
        for( const auto& item : odfitems){
            items.push_back(item);
        }
        std::vector<Resource> finalList;
        for(ODFItem& item : items) {
            if ( item.resolveNames(names)) {
                finalList.push_back(item);
            }
        }

        kernel()->issues()->silent(false);

        if ( finalList.size() > 0)
            kernel()->issues()->log(QString(TR("Added %1 objects through the ilwis3 connector")).arg( finalList.size()),IssueObject::itMessage);

        return finalList;
    }
    catch(const ErrorObject& err){
        kernel()->issues()->silent(false);
        throw err;
    }
}

bool Ilwis3CatalogExplorer::canUse(const Resource &resource) const
{
        if ( resource.ilwisType() != itCATALOG)
            return false;
        if (resource.url().scheme() == "ilwis")
            return false;
        if ( QFileInfo(resource.url().toLocalFile()).isFile()) // no file can be an ilwis3 catalog.
            return false;
        return true;
}

QString Ilwis3CatalogExplorer::provider() const
{
    return "ilwis3";
}

QFileInfo Ilwis3CatalogExplorer::toLocalFile(const QUrl &datasource) const{
    return QFileInfo(datasource.toLocalFile());
}






