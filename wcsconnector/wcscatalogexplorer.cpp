#include <QString>
#include <QUrl>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QUrlQuery>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "connectorinterface.h"
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
#include "xpathparser.h"
#include "wcsservice.h"
#include "wcscapabilitiesparser.h"
#include "wcsdescribecoverage.h"

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
    WebCoverageService Wcs(serviceUrl);
    WcsResponse *gcresponse = Wcs.getCapabilities();
    WcsCapabilitiesParser gcparser(gcresponse, serviceUrl);
    std::map<QString, Resource> rastercoverages;
    gcparser.parse(rastercoverages);

    QString ids ;
    for(const auto& resource : rastercoverages){
        if ( ids != "")
            ids += ",";
        ids += resource.second.name();

    }
    QUrlQuery query(serviceUrl);
    WcsResponse *dcresponse = Wcs.getDescribeCoverage(query,ids);
    WcsDescribeCoverage dcparser(dcresponse, serviceUrl);
    dcparser.parse(rastercoverages);

    std::vector<Resource> resources;
    for(auto cov : rastercoverages)
        resources.push_back(cov.second);
    mastercatalog()->addItems(resources);

    return resources;
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

