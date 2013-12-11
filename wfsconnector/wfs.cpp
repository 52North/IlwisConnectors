#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

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
#include "wfsconnector.h"
#include "wfsresponse.h"
#include "wfs.h"

using namespace Ilwis;
using namespace Wfs;


WebFeatureService::WebFeatureService(QUrl wfsUrl): _resource(wfsUrl)
{
}

WfsResponse *WebFeatureService::getCapabilities()
{
    QUrl getCapabilitiesUrl = _resource;
    return performSyncRequest(getCapabilitiesUrl);
}

WfsResponse *WebFeatureService::describeFeatureType()
{
    QUrl getCapabilitiesUrl = _resource;

    // TODO: adjust to DescribeFeatureType
    //WfsResponse *response = performAsyncRequest(getCapabilitiesUrl);

}

WfsResponse *WebFeatureService::getFeature()
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
    QNetworkReply *reply = response->performRequest(request);

    return response;
}


