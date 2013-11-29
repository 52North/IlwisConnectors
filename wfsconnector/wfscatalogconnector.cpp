#include <QString>
#include <QUrl>
#include <QScopedPointer>
#include <QSharedPointer>
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


    return false;
}

bool WfsCatalogConnector::canUse(const Resource &resource) const
{
    return false;
}

QString WfsCatalogConnector::provider() const
{
    return "wfs";
}




