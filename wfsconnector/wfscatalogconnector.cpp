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
    if (!isValidWfsUrl(resource.url()))
        return false;
    return true;
}

QString WfsCatalogConnector::provider() const
{
    return "wfs";
}

bool WfsCatalogConnector::isValidWfsUrl(QUrl url) const
{
    QUrlQuery query(url);
    lowerCaseKeys(query);

    bool isHttpRequest = url.scheme().startsWith("http");
    bool isWfsRequest = isExpectedValue(query.queryItemValue("service"), "wfs");
    bool isSupportedVersion = isExpectedValue(query.queryItemValue("acceptversions"), "1.1.0");
    return isHttpRequest && isWfsRequest && isSupportedVersion;
}

void WfsCatalogConnector::lowerCaseKeys(QUrlQuery &query) const
{
    for (QPair<QString,QString> kvm : query.queryItems()) {
        query.removeQueryItem(kvm.first);
        query.addQueryItem(kvm.first.toLower(), kvm.second);
    }
}

/**
 * @brief WfsCatalogConnector::hasParameterValue checks if a certain parameter value is present.
 * @param actual the actual value to check.
 * @param expected the expected value.
 * @return true if the actual parameter is equal (incasesensitive) to the expected value.
 */
bool WfsCatalogConnector::isExpectedValue(QString actual, QString expected) const
{
    if (expected != "" && actual == "") {
        return false;
    }
    return actual.compare(expected, Qt::CaseInsensitive) == 0;
}




