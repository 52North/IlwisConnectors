#include <QString>
#include <QUrl>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QUrlQuery>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coverage.h"
#include "polygon.h"
#include "columndefinition.h"
#include "geometry.h"
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

    QStringList filters; // empty filter
    std::vector<QUrl> getFeatureUrls = containerConnector()->sources(filters, ContainerConnector::foFULLPATHS);

    QList<Resource> finalList;
    foreach (QUrl url, getFeatureUrls) {
        Resource featureUrl(url, itCOVERAGE);
        finalList.push_back(featureUrl);
    }

    // TODO: eventually add feature description somewhere

    mastercatalog()->addItems(finalList);
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
    QList<QPair<QString,QString>> queryItems = query.queryItems();
    query.setQueryItems(lowerCaseKeys(queryItems));

    bool isHttpRequest = url.scheme().startsWith("http");
    bool isWfsRequest = isExpectedValue(query.queryItemValue("service"), "wfs");
    bool isSupportedVersion = isExpectedValue(query.queryItemValue("acceptversions"), "1.1.0");
    return isHttpRequest && isWfsRequest && isSupportedVersion;
}

QList<QPair<QString,QString>> WfsCatalogConnector::lowerCaseKeys(QList<QPair<QString,QString>> queryItems) const {
    QList<QPair<QString,QString>> loweredKeysItems;
    for(int i = 0; i < queryItems.length(); i++) {
        QPair<QString,QString> item = queryItems[i];
        item.first = item.first.toLower();
        loweredKeysItems.append(item);
    }
    return loweredKeysItems;
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
    return actual.compare(expected, Qt::CaseInsensitive);
}




