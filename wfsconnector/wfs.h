#ifndef WFS_H
#define WFS_H

#include "wfsconnector_global.h"
#include "wfsresponse.h"

class QNetworkAccessManager;
class QUrl;

namespace Ilwis {
namespace  Wfs {

class WFSCONNECTORSHARED_EXPORT WebFeatureService
{

public:
    WebFeatureService(QUrl wfsUrl);

    WfsResponse *getCapabilities();
    WfsResponse *describeFeatureType();
    WfsResponse *getFeature(QString typeName);
    QUrl createGetFeatureUrl(QString typeName);

private:
    QUrl _resource;

    WfsResponse *performSyncRequest(QUrl request);
    WfsResponse *performAsyncRequest(QUrl request);
};

}
}

#endif // WFS_H
