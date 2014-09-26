#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coverage.h"
#include "columndefinition.h"
#include "record.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "WcsResponse.h"
#include "wcsutils.h"
#include "wcs.h"

using namespace Ilwis;
using namespace Wcs;

WebCoverageService::WebCoverageService(QUrl baseUrl): _resource(baseUrl)
{
}

QUrl WebCoverageService::url() const
{
    return _resource;
}

WcsResponse *WebCoverageService::getCapabilities(bool async) const
{
    QUrlQuery query(_resource);
    return performRequest(query, "GetCapabilities", async);
}

WcsResponse *WebCoverageService::getDescribeCoverage(QUrlQuery query, const QString& id, bool async) const
{
    return performRequest(query, "DescribeCoverage", async, id);
}

WcsResponse *WebCoverageService::getCoverage(QUrlQuery query, const QString& id, bool async) const
{
    return performRequest(query, "GetCoverage", async, id);
}

WcsResponse *WebCoverageService::performRequest(QUrlQuery query, QString request, bool async, const QString& id) const
{
    WcsUtils::lowerCaseKeys(query);
    query.removeQueryItem("request");
    query.addQueryItem("request", request);
    query.removeQueryItem("version");
    query.addQueryItem("version", "1.0.0");
    if ( request == "DescribeCoverage"){
        query.addQueryItem("identifiers", id);
    }
    QUrl serviceUrl = _resource;
    serviceUrl.setQuery(query);

    QString www = serviceUrl.toString();
    if (!async) {
        return performSyncRequest(serviceUrl);
    } else {
        return performAsyncRequest(serviceUrl);
    }
}

WcsResponse *WebCoverageService::performSyncRequest(QUrl requestUrl) const
{
    QNetworkRequest request(requestUrl);
    WcsResponse *response = new WcsResponse;
    QNetworkReply *reply = response->performRequest(request, false);

    QEventLoop loop; // waits for request to complete
    QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
    loop.exec();

    response->readResponse(reply);
    return response;
}

WcsResponse *WebCoverageService::performAsyncRequest(QUrl requestUrl) const
{
    QNetworkRequest request(requestUrl);
    WcsResponse *response = new WcsResponse;
    // response registers an async callback
    response->performRequest(request);
    return response;
}


