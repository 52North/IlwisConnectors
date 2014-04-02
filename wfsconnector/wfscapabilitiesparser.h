#ifndef WFSCAPABILITIESPARSER_H
#define WFSCAPABILITIESPARSER_H

#include "wfsconnector_global.h"

#include <QList>

class QUrl;
class XPathParser;
class QXmlItem;

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCapabilitiesParser
{
public:
    WfsCapabilitiesParser(WfsResponse *response, QUrl wfsUrl);
    ~WfsCapabilitiesParser();

    void parseFeatures(std::vector<Ilwis::Resource> &wfsFeaturesTypes);

private:
    QUrl _url;
    XPathParser *_parser;

    void createGetFeatureUrl(const QString &featureType, QUrl &rawUrl, QUrl &normalizedUrl) const;
    void parseFeature(QXmlItem &item, WfsFeature &feature) const;
    Coordinate createCoordinateFromWgs84LatLon(QString latlon) const;

};
}
}

#endif // WFSCAPABILITIESPARSER_H
