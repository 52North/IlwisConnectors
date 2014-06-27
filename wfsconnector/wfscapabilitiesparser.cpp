#include <sstream>

#include <QUrl>
#include <QString>
#include <QXmlQuery>
#include <QXmlStreamReader>
#include <QXmlResultItems>

#include "kernel.h"
#include "geometries.h"
#include "wfsresponse.h"
#include "wfsfeature.h"
#include "wfscapabilitiesparser.h"
#include "xpathparser.h"
#include "wfsutils.h"

using namespace Ilwis;
using namespace Wfs;

WfsCapabilitiesParser::WfsCapabilitiesParser(WfsResponse *response, QUrl wfsUrl): _url(wfsUrl)
{
    _parser = new XPathParser(response->device());
    _parser->addNamespaceMapping("wfs", "http://www.opengis.net/wfs");
    _parser->addNamespaceMapping("ows", "http://www.opengis.net/ows");
}

WfsCapabilitiesParser::~WfsCapabilitiesParser()
{
    delete _parser;
}

void WfsCapabilitiesParser::parseFeatures(std::vector<Resource> &wfsFeatures)
{
    QXmlResultItems results;
    UPXmlQuery& query = _parser->queryFromRoot("//wfs:WFS_Capabilities/wfs:FeatureTypeList/wfs:FeatureType");

    if (query->isValid()) {
        query->evaluateTo( &results);
        QXmlItem item(results.next());
        while (!item.isNull()) {
            WfsFeature feature;
            parseFeature(item, feature);
            wfsFeatures.push_back(feature);
            item = results.next();
        }
        if (results.hasError()) {
            ERROR0(TR(QString("XPath evaluation failed.")));
        }
    }
}

QString WfsCapabilitiesParser::valueOf(QXmlItem &item, const QString& xpathQuqery) const{
    QString value;
    UPXmlQuery& query = _parser->queryRelativeFrom(item, xpathQuqery);
    query->evaluateTo(&value);
    value = value.trimmed();

    return value;
}

void WfsCapabilitiesParser::parseFeature(QXmlItem &item, WfsFeature &feature) const
{

    QUrl rawUrl, normalizedUrl;
    feature = WfsFeature(rawUrl, normalizedUrl);
    feature.name(valueOf(item, "./wfs:Name/string()"), false);
    createGetFeatureUrl(feature.name(), rawUrl, normalizedUrl);
    feature.setTitle(valueOf(item, "./wfs:Title/string()"));
    feature.setTitle(valueOf(item, "./wfs:Abstract/string()"));
    feature.setTitle(WfsUtils::normalizeEpsgCode(valueOf(item, "./wfs:DefaultSRS/string()")));

    QString llText = valueOf(item, "./ows:WGS84BoundingBox/ows:LowerCorner/string()");
    QString urText = valueOf(item, "./ows:WGS84BoundingBox/ows:UpperCorner/string()");

    Coordinate ll = createCoordinateFromWgs84LatLon(llText);
    Coordinate ur = createCoordinateFromWgs84LatLon(urText);
    Envelope envelope(ll, ur);
    feature.setBBox(envelope);
}
void WfsCapabilitiesParser::createGetFeatureUrl(const QString& featureName, QUrl& rawUrl, QUrl& normalizedUrl) const
{
    QUrlQuery query;
    query.addQueryItem("service", "WFS");
    query.addQueryItem("version", "1.1.0");
    query.addQueryItem("request", "GetFeature");
    query.addQueryItem("typeName", featureName);
    rawUrl = _url; // copy
    rawUrl.setQuery(query);
    normalizedUrl = _url.toString(QUrl::RemoveQuery) + "/" + featureName;
}

Coordinate WfsCapabilitiesParser::createCoordinateFromWgs84LatLon(QString latlon) const
{
    int splitIndex = latlon.indexOf(" ");
    QString lon = latlon.left(splitIndex).trimmed();
    QString lat = latlon.mid(splitIndex + 1).trimmed();
    Coordinate coords(lon.toDouble(), lat.toDouble());
    return coords;
}


