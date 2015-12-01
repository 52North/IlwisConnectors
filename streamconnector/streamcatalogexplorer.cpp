#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "catalogconnector.h"
#include "raster.h"
#include "catalog.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "rawconverter.h"
#include "ilwiscontext.h"
#include "streamcatalogexplorer.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "versioneddatastreamfactory.h"

using namespace Ilwis;
using namespace Stream;

REGISTER_CATALOGEXPLORER(StreamCatalogExplorer)

StreamCatalogExplorer::StreamCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{

}

std::vector<Resource> StreamCatalogExplorer::loadItems(const IOOptions &)
{
    QStringList sfilters;
    sfilters << "*.ilwis" ;
    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);
    std::vector<Resource> items;
    for(auto url : files){
        QFile file(url.toLocalFile());
        QDataStream stream(&file);
        IlwisTypes tp;
        QString version;
        stream >> tp;
        stream >> version;

//        IlwisObject *obj = createType(tp);
//        VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
//        std::unique_ptr<VersionedSerializer> serializer(factory->create(version,tp,stream));
//        if (!serializer)
//            continue;
//        serializer->loadMetaData(obj,IOOptions());
        Resource res(url, tp);
        items.push_back(res);

    }

    return items;

}

IlwisObject *StreamCatalogExplorer::createType(IlwisTypes tp){
    switch(tp){
    case itFEATURE:
        return new FeatureCoverage();
    case itRASTER:
        return new RasterCoverage();
    case itTABLE:
        return new FlatTable();
    }
    return 0;
}

bool StreamCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if ( QFileInfo(resource.url().toLocalFile()).suffix() == "ilwis") // no file can be an ods folder catalog.
        return true;
    return false;
}

QString StreamCatalogExplorer::provider() const
{
    return "ilwis"    ;
}

Ilwis::CatalogExplorer *StreamCatalogExplorer::create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options)
{
    return new StreamCatalogExplorer(resource, options);
}
