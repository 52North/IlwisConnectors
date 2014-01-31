#ifndef WFSCAPABILITIESPARSER_H
#define WFSCAPABILITIESPARSER_H

#include "wfsconnector_global.h"

#include <QList>

class QUrl;
class XmlParser;

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCapabilitiesParser
{
public:
    WfsCapabilitiesParser(WfsResponse *response, QUrl wfsUrl);
    ~WfsCapabilitiesParser();

    void parseFeatures(QList<WfsFeature> &wfsFeaturesTypes);

private:
    QUrl _url;
    QXmlStreamReader *_reader;
    XmlParser *_xmlParser;

    void parseFeature(WfsFeature *feature) const;

    QUrl createGetFeatureUrl(QString featureType) const;
    Coordinate createCoordinateFromWgs84LatLon(QString latlon) const;
    QString normalizeEpsgCode(QString epsgCode) const ;

};
}
}

#endif // WFSCAPABILITIESPARSER_H
