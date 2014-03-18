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
#include "geos/geom/Geometry.h"
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
#include "attributerecord.h"
#include "feature.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "wfsconnector.h"
#include "wfs.h"
#include "wfsutils.h"


using namespace Ilwis;
using namespace Wfs;


WfsConnector::WfsConnector(const Resource &resource, bool load) : IlwisObjectConnector(resource, load)
{
}


QString WfsConnector::provider() const
{
    return QString("wfs");
}








