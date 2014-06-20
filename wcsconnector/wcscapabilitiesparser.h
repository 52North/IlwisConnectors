#ifndef WFSCAPABILITIESPARSER_H
#define WFSCAPABILITIESPARSER_H

#include <QList>

class QUrl;
class XPathParser;
class QXmlItem;

namespace Ilwis {
namespace Wcs {

class WcsCapabilitiesParser
{
public:
    WcsCapabilitiesParser(WcsResponse *response, QUrl wcsUrl);
    ~WcsCapabilitiesParser();

    void parse(std::vector<Ilwis::Resource> &resources) const;


private:
    QUrl _url;
    std::unique_ptr<XPathParser> _parser;

    Coordinate createCoordinateFromWgs84LatLon(QString latlon) const;
    void parseCoverage(QXmlItem &item, WcsItem &coverage) const;
};
}
}

#endif // WFSCAPABILITIESPARSER_H
