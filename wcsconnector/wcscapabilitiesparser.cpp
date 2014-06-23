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
#include "xpathparser.h"
#include "wcsservice.h"
#include "wcscapabilitiesparser.h"
#include "wcsutils.h"

using namespace Ilwis;
using namespace Wcs;

WcsCapabilitiesParser::WcsCapabilitiesParser(WcsResponse *response, QUrl wcsUrl): WcsService(response, wcsUrl)
{

}

WcsCapabilitiesParser::~WcsCapabilitiesParser()
{
}

void WcsCapabilitiesParser::parse(std::map<QString, Resource>& resources) const
{
    QXmlResultItems results;

//    QString describeCoverage;
//    UPXmlQuery& queryDC = _parser->queryFromRoot("//wcs:Capabilities/ows:OperationsMetadata/ows:Operation[@name='DescribeCoverage']/ows:DCP/ows:HTTP/ows:Get/@xlink:href/string()");
//    if (queryDC->isValid()) {
//        queryDC->evaluateTo( &describeCoverage);
//    }



    UPXmlQuery& query = _parser->queryFromRoot("//wcs:Capabilities/wcs:Contents/wcs:CoverageSummary");
    if (query->isValid()) {
        query->evaluateTo( &results);
        QXmlItem item(results.next());
        while (!item.isNull()) {
            Resource coverage;
            parseCoverage(item, coverage);
            resources[coverage.name()] = coverage;
            item = results.next();
        }
        if (results.hasError()) {
            ERROR0(TR(QString("XPath evaluation failed.")));
        }
    }



}

void WcsCapabilitiesParser::parseCoverage(QXmlItem &item, Resource &coverage) const
{
    QString name;
    UPXmlQuery& query = _parser->queryRelativeFrom(item, "./ows:Title/string()");
    query->evaluateTo( &name);
    coverage.name(name.trimmed(), false);


    UPXmlQuery& query2 = _parser->queryRelativeFrom(item, "./ows:Abstract/string()");
    query2->evaluateTo( &name);
    coverage.setDescription(name.trimmed());

    UPXmlQuery& query3 = _parser->queryRelativeFrom(item, "./wcs:Identifier/string()");
    query3->evaluateTo( &name);
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


