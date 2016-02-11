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
#include "workflow/workflow.h"
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
        QString path = url.toLocalFile();
        if ( QFileInfo(path).isDir())
            continue;

        QFile file(path);
        if ( file.open(QIODevice::ReadOnly)){
            QDataStream stream(&file);
            IlwisTypes tp;

            stream >> tp;
            file.close();

            if ( tp == itUNKNOWN)
                continue;
            Resource res(url, tp);
            if ( tp == itWORKFLOW){
                IWorkflow wf;
                wf.prepare(res);
                wf->createMetadata();
                Resource res2 = wf->source();
                res2.code(res.code()); //code comes from other machine or possibly older instance which might have different id's
                items.push_back(res2);
            }
        }
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
    case itWORKFLOW:
        return new Workflow();
    }
    return 0;
}

bool StreamCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if ( resource.url().scheme() == "file")
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
