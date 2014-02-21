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

void WfsCapabilitiesParser::parseFeatures(QList<WfsFeature> &wfsFeatures)
{
    QXmlResultItems results;
    QXmlQuery *query = _parser->queryFromRoot("//wfs:WFS_Capabilities/wfs:FeatureTypeList/wfs:FeatureType");

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

void WfsCapabilitiesParser::parseFeature(QXmlItem &item, WfsFeature &feature) const
{
    QXmlQuery *query;

    QString name;
    query = _parser->queryRelativeFrom(item, "./wfs:Name/string()");
    query->evaluateTo( &name);
    QString type = name.trimmed();

    feature = WfsFeature(createGetFeatureUrl(type));
    feature.setName(type, false);

    QString title;
    query = _parser->queryRelativeFrom(item, "./wfs:Title/string()");
    query->evaluateTo( &title);
    feature.setTitle(title.trimmed());

    QString abstract;
    query = _parser->queryRelativeFrom(item, "./wfs:Abstract/string()");
    query->evaluateTo( &abstract);
    feature.setTitle(abstract.trimmed());

    QString srs;
    query = _parser->queryRelativeFrom(item, "./wfs:DefaultSRS/string()");
    query->evaluateTo( &srs);
    feature.setTitle(WfsUtils::normalizeEpsgCode(srs));

    QString llText;
    query = _parser->queryRelativeFrom(item, "./ows:WGS84BoundingBox/ows:LowerCorner/string()");
    query->evaluateTo( &llText);

    QString urText;
    query = _parser->queryRelativeFrom(item, "./ows:WGS84BoundingBox/ows:UpperCorner/string()");
    query->evaluateTo( &urText);

    Coordinate ll = createCoordinateFromWgs84LatLon(llText);
    Coordinate ur = createCoordinateFromWgs84LatLon(urText);
    Envelope envelope(ll, ur);
    feature.setBBox(envelope);
}

QUrl WfsCapabilitiesParser::createGetFeatureUrl(QString featureName) const
{
    QUrlQuery query;
    query.addQueryItem("service", "WFS");
    query.addQueryItem("version", "1.1.0");
    query.addQueryItem("request", "GetFeature");
    query.addQueryItem("featureName", featureName);
    QUrl getFeatureUrl = _url; // copy
    getFeatureUrl.setQuery(query);
    return getFeatureUrl;
}

Coordinate WfsCapabilitiesParser::createCoordinateFromWgs84LatLon(QString latlon) const
{
    int splitIndex = latlon.indexOf(" ");
    QString lon = latlon.left(splitIndex).trimmed();
    QString lat = latlon.mid(splitIndex + 1).trimmed();
    Coordinate coords(lon.toDouble(), lat.toDouble());
    return coords;
}


