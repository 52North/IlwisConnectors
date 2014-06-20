#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coverage.h"
#include "columndefinition.h"
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

WcsResponse *WebCoverageService::describeCoverage(QUrlQuery query, bool async) const
{
    return performRequest(query, "DescribeCoverage", async);
}

WcsResponse *WebCoverageService::getCoverage(QUrlQuery query, bool async) const
{
    return performRequest(query, "GetCoverage", async);
}

WcsResponse *WebCoverageService::performRequest(QUrlQuery query, QString wfsRequest, bool async) const
{
    WcsUtils::lowerCaseKeys(query);
    query.removeQueryItem("request");
    query.addQueryItem("request", wfsRequest);
    if (wfsRequest == "GetCapabilities") {
        query.removeQueryItem("acceptversions");
        query.addQueryItem("acceptversions", "1.1.0");
    } else {
        query.removeQueryItem("version");
        query.addQueryItem("version", "1.1.0");
    }
    QUrl wfsUrl = _resource;
    wfsUrl.setQuery(query);

    QString www = wfsUrl.toString();
    if (!async) {
        return performSyncRequest(wfsUrl);
    } else {
        return performAsyncRequest(wfsUrl);
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
    //QByteArray bytes = reply->readAll();
    //QString str = QString::fromUtf8(bytes.data(), bytes.size());
    //int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
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


