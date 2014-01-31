#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coverage.h"
#include "columndefinition.h"
#include "geometry.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "wfsconnector.h"
#include "wfsresponse.h"
#include "wfs.h"

using namespace Ilwis;
using namespace Wfs;

WebFeatureService::WebFeatureService(QUrl baseUrl): _resource(baseUrl)
{
}

QUrl WebFeatureService::url()
{
    return _resource;
}

WfsResponse *WebFeatureService::getCapabilities(bool async)
{
    if (!async) {
        return performSyncRequest(_resource);
    } else {
        return performAsyncRequest(_resource);
    }
}

WfsResponse *WebFeatureService::describeFeatureType(QUrlQuery query, bool async)
{
    QUrl describeFeatureUrl = _resource;
    describeFeatureUrl.setQuery(query);

    // TODO: do an async request
    //return performAsyncRequest(getCapabilitiesUrl);

    return performSyncRequest(describeFeatureUrl);

}

WfsResponse *WebFeatureService::getFeature(QUrlQuery query, bool async)
{
    QUrl getCapabilitiesUrl = _resource;

    // TODO: adjust to GetFeature
    //WfsResponse *response = performAsyncRequest(getCapabilitiesUrl);
}

WfsResponse *WebFeatureService::performSyncRequest(QUrl requestUrl)
{
    QNetworkRequest request(requestUrl);
    WfsResponse *response = new WfsResponse;
    QNetworkReply *reply = response->performRequest(request, false);

    QEventLoop loop; // waits for request to complete
    QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
    loop.exec();

    response->readResponse(reply);
    return response;
}

WfsResponse *WebFeatureService::performAsyncRequest(QUrl requestUrl)
{
    QNetworkRequest request(requestUrl);
    WfsResponse *response = new WfsResponse;
    // response registers an async callback
    response->performRequest(request);
    return response;
}


