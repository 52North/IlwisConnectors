#include <QString>
#include <QXmlQuery>
#include <QXmlStreamReader>
#include <QXmlResultItems>
#include <QNetworkReply>
#include "kernel.h"
#include "geometries.h"
#include "wcsresponse.h"
#include "xpathparser.h"
#include "wcsutils.h"
#include "wcsservice.h"
#include "wcsdescribecoverage.h"

using namespace Ilwis;
using namespace Wcs;

WcsDescribeCoverage::WcsDescribeCoverage()
{
}

WcsDescribeCoverage::WcsDescribeCoverage(WcsResponse *response, QUrl wcsUrl) : WcsService(response, wcsUrl)
{

}

WcsDescribeCoverage::~WcsDescribeCoverage()
{
}

void WcsDescribeCoverage::parse(std::map<QString, Resource>& rastercoverages) const
{
    QXmlResultItems results;


    UPXmlQuery& query = _parser->queryFromRoot("//wcs:CoverageDescriptions/wcs:CoverageDescription");
    if (query->isValid()) {
        query->evaluateTo( &results);
        QXmlItem item(results.next());
        while (!item.isNull()) {
            item = results.next();
            parseCoverage(item, rastercoverages);
        }
        if (results.hasError()) {
            ERROR0(TR(QString("XPath evaluation failed.")));
        }
    }

}

void WcsDescribeCoverage::parseCoverage(QXmlItem &item, std::map<QString, Resource>& rastercoverages) const
{
    QString name = valueOf(item, "./ows:Title/string()");
    auto iter = rastercoverages.find(name);
    if ( iter == rastercoverages.end())
        return;
    Resource& resource = iter->second;
    QString crs = valueOf(item, "./wcs:Domain/wcs:SpatialDomain/wcs:GridCRS/wcs:GridBaseCRS/string()");
    QStringList parts = crs.split(":");
    QString cscode=sUNDEF, srscode = parts[6];
    QString authority = parts[4];
    if (authority.toLower() == "epsg"){
        cscode = "code=epsg:" + srscode;
    }else if ( authority.toLower() == "ogc"){
        if ( srscode.toLower() == "crs84"){
            cscode = "code=epsg:4326";
        }    
    }
    if ( cscode != sUNDEF){
        resource.addProperty("coordinatesystem",cscode);
    }
    QString envelopeQuery = "./wcs:Domain/wcs:SpatialDomain/ows:BoundingBox[@crs='" + crs + "']/ows:LowerCorner[1]";
    QString envelope = valueOf(item,envelopeQuery);
    
}

QString WcsDescribeCoverage::valueOf(QXmlItem &item, const QString& xpathQuqery) const{
    QString value;
    UPXmlQuery& query = _parser->queryRelativeFrom(item, xpathQuqery);
    query->evaluateTo(&value);
    value = value.trimmed();
    
    return value;
}
