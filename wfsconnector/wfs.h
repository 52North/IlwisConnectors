#ifndef WFS_H
#define WFS_H

#include <QUrl>
#include <QUrlQuery>

#include "wfsconnector_global.h"
#include "wfsresponse.h"

namespace Ilwis {
namespace  Wfs {

class WFSCONNECTORSHARED_EXPORT WebFeatureService
{

public:
    WebFeatureService(QUrl wfsUrl);

    QUrl url() const;
    WfsResponse *getCapabilities(bool async=false) const;
    WfsResponse *describeFeatureType(QUrlQuery query, bool async=false) const;
    WfsResponse *getFeature(QUrlQuery query, bool async=false) const;

private:
    QUrl _resource;

    WfsResponse *performRequest(QUrlQuery query, QString wfsRequest, bool async=false) const;
    WfsResponse *performSyncRequest(QUrl request) const;
    WfsResponse *performAsyncRequest(QUrl request) const;


};

}
}

#endif // WFS_H
