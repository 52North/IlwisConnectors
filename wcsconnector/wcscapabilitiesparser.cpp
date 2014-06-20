#include <sstream>

#include <QUrl>
#include <QString>
#include <QXmlQuery>
#include <QXmlStreamReader>
#include <QXmlResultItems>
#include <QNetworkReply>
#include "kernel.h"
#include "geometries.h"
#include "wcsresponse.h"
#include "wcsitem.h"
#include "wcscapabilitiesparser.h"
#include "xpathparser.h"
#include "wcsutils.h"

using namespace Ilwis;
using namespace Wcs;

WcsCapabilitiesParser::WcsCapabilitiesParser(WcsResponse *response, QUrl wcsUrl): _url(wcsUrl)
{
    _parser.reset(new XPathParser(response->device()));
    _parser->addNamespaceMapping("wcs", "http://www.opengis.net/wcs/1.1.1");
    _parser->addNamespaceMapping("ows", "http://www.opengis.net/ows/1.1");
}

WcsCapabilitiesParser::~WcsCapabilitiesParser()
{
}

void WcsCapabilitiesParser::parse(std::vector<Resource>& resources) const
{
    QXmlResultItems results;
    QXmlQuery *query = _parser->queryFromRoot("//wcs:Capabilities/wcs:Contents/wcs:CoverageSummary");

    if (query->isValid()) {
        query->evaluateTo( &results);
        QXmlItem item(results.next());
        while (!item.isNull()) {
            WcsItem coverage;
            parseCoverage(item, coverage);
            resources.push_back(coverage);
            item = results.next();
        }
        if (results.hasError()) {
            ERROR0(TR(QString("XPath evaluation failed.")));
        }
    }
}

void WcsCapabilitiesParser::parseCoverage(QXmlItem &item, WcsItem &coverage) const
{
    QXmlQuery *query;

    QString name;
    query = _parser->queryRelativeFrom(item, "./ows:Title/string()");
    query->evaluateTo( &name);
    coverage.name(name.trimmed(), false);


    query = _parser->queryRelativeFrom(item, "./ows:Abstract/string()");
    query->evaluateTo( &name);
    coverage.setDescription(name.trimmed());

    query = _parser->queryRelativeFrom(item, "./wcs:Identifier/string()");
    query->evaluateTo( &name);
    coverage.code(name.trimmed());
}

Coordinate WcsCapabilitiesParser::createCoordinateFromWgs84LatLon(QString latlon) const
{
    int splitIndex = latlon.indexOf(" ");
    QString lon = latlon.left(splitIndex).trimmed();
    QString lat = latlon.mid(splitIndex + 1).trimmed();
    Coordinate coords(lon.toDouble(), lat.toDouble());
    return coords;
}


