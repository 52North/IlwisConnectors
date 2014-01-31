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

    QUrl url();
    WfsResponse *getCapabilities(bool async=false);
    WfsResponse *describeFeatureType(QUrlQuery query, bool async=false);
    WfsResponse *getFeature(QUrlQuery query, bool async=false);

private:
    QUrl _resource;

    WfsResponse *performSyncRequest(QUrl request);
    WfsResponse *performAsyncRequest(QUrl request);
};

}
}

#endif // WFS_H
