#include <QUrl>
#include <QString>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include "wfsConnector_global.h"

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "range.h"
#include "domain.h"
#include "datadefinition.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "columndefinition.h"
#include "table.h"
#include "catalog.h"
#include "polygon.h"
#include "geometry.h"
#include "attributerecord.h"
#include "feature.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "wfsconnector.h"
#include "wfs.h"


using namespace Ilwis;
using namespace Wfs;


WfsConnector::WfsConnector(const Resource &resource, bool load) : IlwisObjectConnector(resource, load)
{
}


QString WfsConnector::provider() const
{
    return QString("wfs");
}

bool WfsConnector::loadMetaData(IlwisObject *data)
{
    // TODO: load WFS feature metadata

    return true;
}

IlwisTypes WfsConnector::ilwisType(const QString &name) {
//    QString filename = name;
//    if (name.contains("?") == 0) {
//        filename = name.split("?").front();
//    }
//    QFileInfo inf(filename);
//    bool isCatalog =  inf.isDir();
//    if ( isCatalog)
//        return itCATALOG;

//    QString ext = inf.suffix();
//    if ( ext == "mpr")
//        return itRASTER;
//    if ( ext == "mpa")
//        return itPOLYGON;
//    if ( ext == "mps")
//        return itLINE;
//    if ( ext == "mpp")
//        return itPOINT;
//    if ( ext == "dom")
//        return itDOMAIN;
//    if ( ext == "csy")
//        return itCOORDSYSTEM;
//    if ( ext == "grf")
//        return itGEOREF;
//    if ( ext == "tbt")
//        return itTABLE;
//    if ( ext == "mpl")
//        return itRASTER;
//    if ( ext == "ioc")
//        return itOBJECTCOLLECTION;

    return itFEATURE;
}








