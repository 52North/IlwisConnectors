#ifndef WFS_H
#define WFS_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "pugixml/pugixml.hpp"

namespace Ilwis {
namespace  Wfs {

class WebFeatureService : public QObject
{
    Q_OBJECT

public:
    WebFeatureService(const Resource &resource);

    pugi::xml_parse_result getCapabilities();
    pugi::xml_parse_result describeFeatureType(Resource &featureType);
    pugi::xml_parse_result getFeature(Resource& featureType);

private:
    QNetworkAccessManager* _networkManager;
    Resource& _resource;
};

}
}

#endif // WFS_H
