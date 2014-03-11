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
#include "containerconnector.h"
#include "resource.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "wfscatalogconnector.h"
#include "wfsutils.h"
#include "wfs.h"
#include "wfsresponse.h"
#include "wfsfeature.h"
#include "wfscapabilitiesparser.h"

using namespace Ilwis;
using namespace Wfs;

ConnectorInterface *WfsCatalogConnector::create(const Resource &resource, bool)
{
    return new WfsCatalogConnector(resource);
}

WfsCatalogConnector::WfsCatalogConnector(const Resource &resource) : CatalogConnector(resource)
{
}

WfsCatalogConnector::~WfsCatalogConnector()
{
}

bool WfsCatalogConnector::loadItems()
{
    if (!isValid())
        return false;

    QUrl serviceUrl = source().url();
    WebFeatureService wfs(serviceUrl);
    WfsResponse *response = wfs.getCapabilities();
    WfsCapabilitiesParser parser(response, serviceUrl);

    QList<WfsFeature> wfsFeatures;
    parser.parseFeatures(wfsFeatures);

    QList<Resource> wfsResources;
    std::for_each(wfsFeatures.begin(), wfsFeatures.end(), [&](WfsFeature &feature) {
        wfsResources.push_back(feature);
    });

    mastercatalog()->addItems(wfsResources);
    return true;
}

bool WfsCatalogConnector::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (!WfsUtils::isValidWfsUrl(resource.url()))
        return false;
    return true;
}

QString WfsCatalogConnector::provider() const
{
    return "wfs";
}




