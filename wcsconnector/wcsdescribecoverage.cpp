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


    QString rootq = query("descriptioncoverage");
    UPXmlQuery& query = parser()->queryFromRoot(rootq);
    if (query->isValid()) {
        query->evaluateTo( &results);
        QXmlItem item(results.next());

        while (!item.isNull()) {
            parseCoverage(item, rastercoverages);
            item = results.next();
        }
        if (results.hasError()) {
            ERROR0(TR(QString("XPath evaluation failed.")));
        }
    }

}

void WcsDescribeCoverage::parseCoverage(QXmlItem &item, std::map<QString, Resource>& rastercoverages) const
{
    QString name = valueOf(item, query("getname"));
    if ( name.contains(":")){ // some layers may containan id with a (unspecified) prefix which seperates it form the 'real' identifier
        name = name.mid(name.indexOf(":") + 1);
    }
    auto iter = rastercoverages.find(name);
    if ( iter == rastercoverages.end())
        return;


    Resource& resource = iter->second;
    QString dd = query("gettimeposition");
    QString tt = valueOf(item,dd);
    QString csyQuery = query("getcsy");
    QString crs = valueOf(item, csyQuery);
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
    QString envelopeQuery = QString(query("getlowercorner"));
    QString lowercorner = valueOf(item,envelopeQuery);
    envelopeQuery = QString(query("getuppercorner"));
    QString uppercorner = valueOf(item,envelopeQuery);
    Envelope envelope;
    parts = lowercorner.split(" ");
    if ( parts.size() == 2){
        Coordinate crdLower = Coordinate(parts[0].toDouble(), parts[1].toDouble());
        parts = uppercorner.split(" ");
        Coordinate crdUpper = Coordinate(parts[0].toDouble(), parts[1].toDouble());
        envelope = Envelope(crdLower, crdUpper);
        QVariant var;
        var.setValue<Ilwis::Envelope>(envelope);
        resource.addProperty("envelope",var);
    }
    double pixsizex=1, pixsizey=1;
    QString pixsquery = query("getpixelsizes");
    if ( currentVersion() == "1.1.0"){
        QString pixdimensions = valueOf(item,pixsquery);
        parts = pixdimensions.split(" ");
        pixsizex = std::abs(parts[0].toDouble());
        pixsizey = std::abs(parts[3].toDouble());
    }else if ( currentVersion() == "1.0.0"){
        QString pixxquery = pixsquery + "[0]/string()";
        QString pixdimensions = valueOf(item,pixxquery);
        parts = pixdimensions.split(" ");
    }

    Size<> sz(envelope.xlength() / pixsizex, envelope.ylength() / pixsizey,1 );
    QVariant var;
    var.setValue<Ilwis::BoundingBox>(BoundingBox(sz));
    resource.addProperty("size", var);
    // describecoverage does not contain reliable information about the datatype, generic value is a good default
    // during getcoverage we may change this to the actual datatype.
    resource.addProperty("domain","value");

    
}


