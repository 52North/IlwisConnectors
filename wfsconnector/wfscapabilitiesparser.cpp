#include <sstream>

#include <QUrl>
#include <QString>
#include <QXmlStreamReader>

#include "pugixml/pugixml_global.h"
#include "pugixml/pugixml.hpp"

#include "kernel.h"
#include "geometries.h"
#include "wfsresponse.h"
#include "wfsfeature.h"
#include "wfscapabilitiesparser.h"
#include "xmlparser.h"


using namespace Ilwis;
using namespace Wfs;

WfsCapabilitiesParser::WfsCapabilitiesParser(WfsResponse *response, QUrl wfsUrl): _url(wfsUrl)
{
//    std::istringstream cs(response->getContent().toStdString());
//    _doc.load(cs);
    _reader = response->xmlReader();
    _xmlParser = new XmlParser(_reader);
    _xmlParser->addNamespaceMapping("wfs", "http://www.opengis.net/wfs");
    _xmlParser->addNamespaceMapping("ows", "http://www.opengis.net/ows");

}

WfsCapabilitiesParser::~WfsCapabilitiesParser()
{

}

void WfsCapabilitiesParser::parseFeatures(QList<WfsFeature> &wfsFeatures)
{
    if (_xmlParser->startParsing("wfs:WFS_Capabilities")) {
        QString current = _reader->name().toString();
        if (_xmlParser->moveTo("wfs:FeatureTypeList")) {
            current = _reader->name().toString();
            if (_xmlParser->moveTo("wfs:FeatureType", false)) {
                current = _reader->name().toString();
                do {
                    WfsFeature feature;
                    parseFeature( &feature);
                    wfsFeatures.push_back(feature);
                    _reader->readNextStartElement();
                    current = _reader->name().toString();
                } while (_xmlParser->isAtBeginningOf("wfs:FeatureType"));
            }
        }
    }

    if (_reader->hasError()) {
        QString error(_reader->error());
        QString message(_reader->errorString());
        ERROR2(TR(QString("Unable to parse features from capabilities")).append(" (%1): %2"), error, message);
    }
}

void WfsCapabilitiesParser::parseFeature(WfsFeature *feature) const
{
    while (!_xmlParser->isAtEndOf("wfs:FeatureType")) {

        _reader->readNext();
        if (_reader->isStartElement()) {
            if (_reader->name() == "Keywords") {
                _reader->skipCurrentElement();
                continue;
            }
            if (_reader->name() == "Name") {
                QString name = _reader->readElementText();
                feature->setUrl(createGetFeatureUrl(name));
                continue;
            }
            if (_reader->name() == "Title") {
                feature->setTitle(_reader->readElementText());
                continue;
            }
            if (_reader->name() == "Abstract") {
                feature->setAbstract(_reader->readElementText());
                continue;
            }
            if (_reader->name() == "DefaultSRS") {
                QString srs = _reader->readElementText();
                feature->setCode(normalizeEpsgCode(srs));
                continue;
            }
            if (_reader->qualifiedName() == "ows:WGS84BoundingBox") {

                Coordinate ll, ur;
                _reader->readNextStartElement();
                if (_reader->name() == "LowerCorner") {
                    ll = createCoordinateFromWgs84LatLon(_reader->readElementText());
                }
                _reader->readNextStartElement();
                if (_reader->name() == "UpperCorner") {
                    ur = createCoordinateFromWgs84LatLon(_reader->readElementText());
                }
                Envelope envelope(ll, ur);
                feature->setBBox(envelope);
                continue;
            }
        }
    }
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

