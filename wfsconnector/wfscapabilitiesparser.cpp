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
#include "xmlstreamparser.h"
#include "xpathparser.h"


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
    QXmlQuery *query = _parser->parseAbsolute("//wfs:WFS_Capabilities/wfs:FeatureTypeList/wfs:FeatureType");

    if (query->isValid()) {
        query->evaluateTo( &results);
        QXmlItem item(results.next());
        while (!item.isNull()) {
            WfsFeature feature;

            QString name;
            _parser->parseRelativeFrom(item, "./wfs:Name/string()");
            query->evaluateTo( &name);
            feature.setName(name);


            QString title;
            _parser->parseRelativeFrom(item, "./wfs:Title/string()");
            query->evaluateTo( &title);
            feature.setTitle(title);


            QString abstract;
            _parser->parseRelativeFrom(item, "./wfs:Abstract/string()");
            query->evaluateTo( &abstract);
            feature.setTitle(abstract);


            QString srs;
            _parser->parseRelativeFrom(item, "./wfs:DefaultSRS/string()");
            query->evaluateTo( &srs);
            feature.setTitle(srs);


            QString llText;
            _parser->parseRelativeFrom(item, "./ows:WGS84BoundingBox/ows:LowerCorner/string()");
            query->evaluateTo( &llText);

            QString urText;
            _parser->parseRelativeFrom(item, "./ows:WGS84BoundingBox/ows:UpperCorner/string()");
            query->evaluateTo( &urText);

            QString ll = createCoordinateFromWgs84LatLon(llText);
            QString ur = createCoordinateFromWgs84LatLon(urText);
            Envelope envelope(ll, ur);
            feature->setBBox(envelope);

            wfsFeatures.push_back(feature);
            item = results.next();
        }
        if (results.hasError()) {
            ERROR0(TR(QString("XPath evaluation failed.")));
        }
    }



//    QXmlStreamReader *reader = _parser->reader();
//    if (_parser->startParsing("wfs:WFS_Capabilities")) {
//        QString current = reader->name().toString();
//        if (_parser->moveTo("wfs:FeatureTypeList")) {
//            current = reader->name().toString();
//            if (_parser->moveTo("wfs:FeatureType", false)) {
//                current = reader->name().toString();
//                do {
//                    WfsFeature feature;
//                    parseFeature( &feature, reader);
//                    wfsFeatures.push_back(feature);
//                    reader->readNextStartElement();
//                    current = reader->name().toString();
//                } while (_parser->isAtBeginningOf("wfs:FeatureType"));
//            }
//        }
//    }

//    if (reader->hasError()) {
//        QString error(reader->error());
//        QString message(reader->errorString());
//        ERROR2(TR(QString("Unable to parse features from capabilities")).append(" (%1): %2"), error, message);
//    }
}

//void WfsCapabilitiesParser::parseFeature(WfsFeature *feature, QXmlStreamReader *reader) const
//{
//    while (!_parser->isAtEndOf("wfs:FeatureType")) {

//        reader->readNext();
//        if (reader->isStartElement()) {
//            if (reader->name() == "Keywords") {
//                reader->skipCurrentElement();
//                continue;
//            }
//            if (reader->name() == "Name") {
//                QString name = reader->readElementText();
//                feature->setUrl(createGetFeatureUrl(name));
//                continue;
//            }
//            if (reader->name() == "Title") {
//                feature->setTitle(reader->readElementText());
//                continue;
//            }
//            if (reader->name() == "Abstract") {
//                feature->setAbstract(reader->readElementText());
//                continue;
//            }
//            if (reader->name() == "DefaultSRS") {
//                QString srs = reader->readElementText();
//                feature->setCode(normalizeEpsgCode(srs));
//                continue;
//            }
//            if (reader->qualifiedName() == "ows:WGS84BoundingBox") {

//                Coordinate ll, ur;
//                reader->readNextStartElement();
//                if (reader->name() == "LowerCorner") {
//                    ll = createCoordinateFromWgs84LatLon(reader->readElementText());
//                }
//                reader->readNextStartElement();
//                if (reader->name() == "UpperCorner") {
//                    ur = createCoordinateFromWgs84LatLon(reader->readElementText());
//                }
//                Envelope envelope(ll, ur);
//                feature->setBBox(envelope);
//                continue;
//            }
//        }
//    }
//}

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
    QString lon = latlon.left(splitIndex);
    QString lat = latlon.mid(splitIndex + 1);
    Coordinate coords(lon.toDouble(), lat.toDouble());
    return coords;
}

QString WfsCapabilitiesParser::normalizeEpsgCode(QString epsgCode) const
{
    QString normalizedCode("EPSG:");
    int splitIndex = epsgCode.lastIndexOf(":");
    if (splitIndex == -1) {
        splitIndex = epsgCode.lastIndexOf("/");
    }
    QString code = epsgCode.mid(splitIndex + 1);
    return normalizedCode.append(code);
}

