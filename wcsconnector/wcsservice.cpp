#include <QUrl>
#include "kernel.h"
#include "wcsresponse.h"
#include "xpathparser.h"
#include "wcsservice.h"

using namespace Ilwis;
using namespace Wcs;

WcsService::WcsService()
{
}

WcsService::WcsService(WcsResponse *response, QUrl wcsUrl) : _url(wcsUrl)
{
    QUrlQuery query(wcsUrl);
    _currentVersion = query.queryItemValue("version");
    if ( _currentVersion == "")
        _currentVersion = query.queryItemValue("VERSION");

    _parser.reset(new XPathParser(response->device()));
    if ( _currentVersion == "1.1.0")
        _parser->addNamespaceMapping("wcs", "http://www.opengis.net/wcs/1.1.1");
    if ( _currentVersion == "1.0.0")
        _parser->addNamespaceMapping("wcs", "http://www.opengis.net/wcs");

    _parser->addNamespaceMapping("ows", "http://www.opengis.net/ows/1.1");
    _parser->addNamespaceMapping("xlink", "http://www.w3.org/1999/xlink");
    _parser->addNamespaceMapping("xlink", "http://www.w3.org/1999/xlink");



    QString prefixSpatialDomain = "./wcs:domainSet/wcs:spatialDomain";
    _queries["1.0.0"]["descriptioncoverage"] = "//wcs:CoverageDescription/wcs:CoverageOffering";
    _queries["1.0.0"]["gettimeposition"] = prefixSpatialDomain + "/EnvelopeWithTimePeriod/gml:timePosition[1]/text()";
    _queries["1.0.0"]["getcsy"] = prefixSpatialDomain + "/*[contains(local-name(.),'Envelope')]/@srsName/string()";
    _queries["1.0.0"]["getname"] = "./wcs:name/string()";
    //_queries["1.0.0"]["getlowercorner"] = prefixSpatialDomain + "/*[contains(local-name(.),'Envelope')]/gml:pos[1]/text()";
    _queries["1.0.0"]["getlowercorner"] = prefixSpatialDomain + "/wcs:EnvelopeWithTimePeriod/gml:pos[1]/text()";
    _queries["1.0.0"]["getuppercorner"] = prefixSpatialDomain + "/*[contains(local-name(.),'Envelope')]/gml:pos[2]/string()";
    _queries["1.0.0"]["getpixelsizes"] = prefixSpatialDomain + "/gml:RectifiedGrid/gml:origin/gml:offsetVector";
    _queries["1.0.0"]["getcoverages"] = "//wcs:WCS_Capabilities/wcs:ContentMetadata/wcs:CoverageOfferingBrief";

    prefixSpatialDomain = "./*[local-name(.) = 'Domain']/*[local-name(.) = 'SpatialDomain']";
    _queries["1.1.0"]["descriptioncoverage"] = "//*[local-name(.) = 'CoverageDescriptions']/*[local-name(.) = 'CoverageDescription']";
    _queries["1.1.0"]["getcsy"] = prefixSpatialDomain + "/*[local-name(.) = 'GridCRS']/*[local-name(.) = 'GridBaseCRS']/string()";
    _queries["1.1.0"]["getname"] = "./wcs:Identifier/string()";
    _queries["1.1.0"]["getlowercorner"] = prefixSpatialDomain + "/ows:BoundingBox[@crs='%1']/ows:LowerCorner[1]/string()";
    _queries["1.1.0"]["getuppercorner"] = prefixSpatialDomain + "/ows:BoundingBox[@crs='%1']/ows:UpperCorner[1]/string()";
    _queries["1.1.0"]["getpixelsizes"] = prefixSpatialDomain + "/wcs:GridCRS/wcs:GridOffsets/string()";
    _queries["1.1.0"]["getcoverages"] = "//wcs:Capabilities/wcs:Contents/wcs:CoverageSummary";
}

Ilwis::Wcs::WcsService::~WcsService()
{

}

QString WcsService::valueOf(QXmlItem &item, const QString& xpathQuqery) const{
    QString value;
    UPXmlQuery& query = _parser->queryRelativeFrom(item, xpathQuqery);
    query->evaluateTo(&value);
    value = value.trimmed();

    return value;
}

const UPXPathParser &WcsService::parser() const
{
    return _parser;
}

QString WcsService::currentVersion() const
{
    return _currentVersion;
}

QString WcsService::query(const QString& id) const{
    auto iter = _queries.find(_currentVersion);
    if ( iter != _queries.end()){
        auto iterquery = iter->second.find(id);
        if ( iterquery != iter->second.end())
            return iterquery->second;
    }
    return sUNDEF;
}
