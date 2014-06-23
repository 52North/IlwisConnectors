#ifndef WFSCAPABILITIESPARSER_H
#define WFSCAPABILITIESPARSER_H

#include <QList>

class QUrl;
class XPathParser;
class QXmlItem;

namespace Ilwis {
namespace Wcs {

class WcsCapabilitiesParser : public WcsService
{
public:
    WcsCapabilitiesParser(WcsResponse *response, QUrl wcsUrl);
    ~WcsCapabilitiesParser();

    void parse(std::map<QString, Resource>& resources) const;

private:
    Coordinate createCoordinateFromWgs84LatLon(QString latlon) const;
    void parseCoverage(QXmlItem &item, Ilwis::Resource &coverage) const;
};
}
}

#endif // WFSCAPABILITIESPARSER_H
