#include "pugixml/pugixml.hpp"
#include "featurecoverage.h"
#include "wfs.h"

using namespace Ilwis;
using namespace Wfs;

WebFeatureService::WebFeatureService(const Resource &resource)
{
    _networkManager = new QNetworkAccessManager(this);
}

pugi::xml_parse_result WebFeatureService::getCapabilities()
{
    _networkManager->get(QNetworkRequest(_resource.url()));
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finishedSlot(QNetworkReply*)));
}

pugi::xml_parse_result WebFeatureService::describeFeatureType(Resource &featureType)
{

}

Features WebFeatureService::getFeature(Resource &featureType)
{

}

void WebFeatureService::finishedSlot(QNetworkReply *reply)
{

}

