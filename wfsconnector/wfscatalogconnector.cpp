#include <QString>
#include <QUrl>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QUrlQuery>

#include "ilwis.h"
#include "connectorinterface.h"
#include "identity.h"
#include "kernel.h"
#include "containerconnector.h"
#include "resource.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "wfsconnector.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "wfscatalogconnector.h"
#include "wfs.h"

using namespace Ilwis;
using namespace Wfs;

ConnectorInterface *WfsCatalogConnector::create(const Resource &resource, bool) {
    return new WfsCatalogConnector(resource);
}

WfsCatalogConnector::WfsCatalogConnector(const Resource &resource) : CatalogConnector(resource)
{
}


bool WfsCatalogConnector::loadItems()
{
    if (!isValid())
        return false;

    WebFeatureService wfs(_location);

    mastercatalog()->addItems(finalList);

    return false;
}

bool WfsCatalogConnector::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (!isValidWfsUrl(resource.url()))
        return false;
    return true;
}

QString WfsCatalogConnector::provider() const
{
    return "wfs";
}

bool WfsCatalogConnector::isValidWfsUrl(QUrl url) const {
    QUrlQuery query(url);
    bool isHttpRequest = url.scheme().startsWith("http");
    bool isWfsRequest = query.queryItemValue("service").toLower() == "wfs";
    bool isSupportedVersion = query.queryItemValue("acceptVersions") == "1.1.0";
    return isHttpRequest && isWfsRequest && isSupportedVersion;
}




