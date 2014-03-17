#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QDir>
#include <QVector>
#include "identity.h"
#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "resource.h"
#include "mastercatalog.h"
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
#include "foldercatalogexplorer.h"
#include "ilwis3catalogexplorer.h"

using namespace Ilwis;
using namespace Ilwis3;

REGISTER_CATALOGEXPLORER(Ilwis3CatalogExplorer)

CatalogExplorer *Ilwis3CatalogExplorer::create(const Resource &resource,const PrepareOptions& options=PrepareOptions()) {
        return new Ilwis3CatalogExplorer(resource, options);
}

Ilwis3CatalogExplorer::Ilwis3CatalogExplorer(const Resource &resource, const PrepareOptions &options) : FolderCatalogExplorer(resource, true, options)
{
}

inline uint qHash(const QFileInfo& inf ){
    return ::qHash(inf.canonicalFilePath());
}

std::vector<Resource> Ilwis3CatalogExplorer::loadItems()
{
    QVariantList filters = DataFormat::getFormatProperties(DataFormat::fpEXTENSION, itILWISOBJECT,"ilwis3");
    QStringList sfilters;
    for(QVariant& ext : filters)
        sfilters += "*." + ext.toString();
    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    std::set<ODFItem> odfitems;
   // std::vector<Resource> folders;
    QHash<QString, quint64> names;
    foreach(const QUrl& url, files) {
        QFileInfo file = toLocalFile(url);
        IlwisTypes tp = Ilwis3Connector::ilwisType(file.fileName());
        if ( mastercatalog()->url2id(url, tp) == i64UNDEF) {
            if ( tp & itILWISOBJECT ) {
                ODFItem item(file);
                odfitems.insert(item);
                names[url.toString().toLower()] = item.id();
            }
        }

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
    mastercatalog()->addItems(finalList);
   // mastercatalog()->addItems(folders);


    return finalList;
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
    //TODO for non-file based systems
    return QFileInfo(datasource.toLocalFile());
}






