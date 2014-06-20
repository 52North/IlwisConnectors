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
#include "wcscatalogexplorer.h"
#include "wcsutils.h"
#include "wcs.h"
#include "wcsresponse.h"
#include "wcsitem.h"
#include "wcscapabilitiesparser.h"

using namespace Ilwis;
using namespace Wcs;

REGISTER_CATALOGEXPLORER(WcsCatalogExplorer)

CatalogExplorer *WcsCatalogExplorer::create(const Resource &resource, const PrepareOptions &options)
{
    return new WcsCatalogExplorer(resource, options);
}

WcsCatalogExplorer::WcsCatalogExplorer(const Resource &resource, const PrepareOptions &options) : CatalogExplorer(resource,options)
{
}

WcsCatalogExplorer::~WcsCatalogExplorer()
{
}

std::vector<Resource> WcsCatalogExplorer::loadItems()
{
    QUrl serviceUrl = source().url(true);
    QString dumm = serviceUrl.toString();
    WebCoverageService Wcs(serviceUrl);
    WcsResponse *response = Wcs.getCapabilities();
    WcsCapabilitiesParser parser(response, serviceUrl);

    std::vector<Resource> WcsCoverages;
    parser.parse(WcsCoverages);
    mastercatalog()->addItems(WcsCoverages);
    return WcsCoverages;
}

bool WcsCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (!WcsUtils::isValidWcsUrl(resource.url()))
        return false;
    return true;
}

QString WcsCatalogExplorer::provider() const
{
    return "Wcs";
}

QFileInfo WcsCatalogExplorer::toLocalFile(const QUrl &) const
{
    return QFileInfo();
}

