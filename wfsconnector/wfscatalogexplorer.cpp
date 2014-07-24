#include <QString>
#include <QUrl>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QUrlQuery>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coverage.h"
#include "columndefinition.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "connectorinterface.h"
#include "identity.h"
#include "resource.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "wfscatalogexplorer.h"
#include "wfsutils.h"
#include "wfs.h"
#include "wfsresponse.h"
#include "wfsfeature.h"
#include "wfscapabilitiesparser.h"

using namespace Ilwis;
using namespace Wfs;

REGISTER_CATALOGEXPLORER(WfsCatalogExplorer)

CatalogExplorer *WfsCatalogExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new WfsCatalogExplorer(resource, options);
}

WfsCatalogExplorer::WfsCatalogExplorer(const Resource &resource, const IOOptions &options) : CatalogExplorer(resource,options)
{
}

WfsCatalogExplorer::~WfsCatalogExplorer()
{
}

std::vector<Resource> WfsCatalogExplorer::loadItems()
{
    QUrl serviceUrl = source().url();
    WebFeatureService wfs(serviceUrl);
    WfsResponse *response = wfs.getCapabilities();
    WfsCapabilitiesParser parser(response, serviceUrl);

    std::vector<Resource> wfsFeatures;
    parser.parseFeatures(wfsFeatures);
    mastercatalog()->addItems(wfsFeatures);
    return wfsFeatures;
}

bool WfsCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (!WfsUtils::isValidWfsUrl(resource.url()))
        return false;
    return true;
}

QString WfsCatalogExplorer::provider() const
{
    return "wfs";
}

QFileInfo WfsCatalogExplorer::toLocalFile(const QUrl &datasource) const
{
    QFileInfo fileInfo;
    return fileInfo;
}

