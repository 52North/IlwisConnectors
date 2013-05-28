#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QDir>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "inifile.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "boostext.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "georefconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *GeorefConnector::create(const Resource &item, bool load) {
    return new GeorefConnector(item, load);

}


GeorefConnector::GeorefConnector(const Resource &item, bool load) : Ilwis3Connector(item, load) {

}


bool GeorefConnector::loadGeoref(const IniFile &odf, IlwisObject *data ) {
    QString type = odf.value("GeoRef","Type");
    if ( type == "?") {
        kernel()->issues()->log(TR("Invalid Georef section in %1").arg(odf.fileinfo().baseName()));
        return false;
    }
    bool ok1, ok2;
    quint32 lines = odf.value("GeoRef","Lines").toInt(&ok1);
    quint32 columns = odf.value("GeoRef","Columns").toInt(&ok2);
    if ( !(ok1 & ok2)) {
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Lines/Columns","Georeference");
    }

    GeoReference *grf = static_cast<GeoReference *>(data);
    grf->size(Size(columns, lines,1));
    if ( type == "GeoRefCorners") {
        //_type = itCORNERSGEOREF;
        return loadGeorefCorners(odf, data);
    } else if ( type == "GeoRefSubMap") {
        QString name = odf.value("GeoRefSubMap","GeoRef");
        QUrl resource = mastercatalog()->name2url(name, itGEOREF);
        IniFile odf;
        odf.setIniFile(resource);
        return loadGeoref(odf,data);
    }
    //TODO tiepoints georef
    return false;
}

bool GeorefConnector::loadMetaData(IlwisObject *data)
{
    Ilwis3Connector::loadMetaData(data);
    IniFile *odf = _odf.data();

    return loadGeoref(*odf, data);
}

bool GeorefConnector::storeMetaData(IlwisObject *obj)
{
    Ilwis3Connector::storeMetaData(obj);
    GeoReference *grf = static_cast<GeoReference *>(obj);
    _odf->setKeyValue("GeoRef","CoordSystem",Resource::toLocalFile(grf->coordinateSystem()->source(),true));
    Size sz = grf->size();
    _odf->setKeyValue("GeoRef","Lines", QString::number(sz.ysize()));
    _odf->setKeyValue("GeoRef","Columns", QString::number(sz.xsize()));
    CornersGeoReference *cgrf = dynamic_cast<CornersGeoReference *>(grf);
    if ( cgrf) {
        _odf->setKeyValue("GeoRef","CornersOfCorners", cgrf->isCornersOfCorners()? "Yes" : "No");
        _odf->setKeyValue("GeoRef","Type", "GeoRefCorners");
        Coordinate cmin = cgrf->envelope().min_corner();
        Coordinate cmax = cgrf->envelope().max_corner();
        _odf->setKeyValue("GeoRefCorners", "MinX", QString::number(cmin.x()));
        _odf->setKeyValue("GeoRefCorners", "MinY", QString::number(cmin.y()));
        _odf->setKeyValue("GeoRefCorners", "MaxX", QString::number(cmax.x()));
        _odf->setKeyValue("GeoRefCorners", "MaxY", QString::number(cmax.y()));

        std::vector<double> matrix = cgrf->matrix();
        _odf->setKeyValue("GeoRefSmpl", "a11", QString::number(matrix[0]));
        _odf->setKeyValue("GeoRefSmpl", "a22", QString::number(matrix[3]));
        _odf->setKeyValue("GeoRefSmpl", "a12", "0");
        _odf->setKeyValue("GeoRefSmpl", "a21", "0");

         std::vector<double> support = cgrf->support();
         _odf->setKeyValue("GeoRefSmpl", "b1", QString::number(support[0]));
         _odf->setKeyValue("GeoRefSmpl", "b2", QString::number(support[1]));

        _odf->store();
        return true;

    }

    return false;

}

bool GeorefConnector::loadGeorefCorners(const IniFile& odf, IlwisObject *data) {
    CornersGeoReference *grf = static_cast<CornersGeoReference *>(data);
    QString csyName = odf.value("GeoRef","CoordSystem");
    QUrl path = mastercatalog()->name2url(csyName, itCOORDSYSTEM);
    ICoordinateSystem csy;
    if(!csy.prepare(path.toLocalFile())) {
        kernel()->issues()->log(TR("Couldnt find coordinate system %1, loading unknown").arg(csyName),IssueObject::itWarning);
        QString res = QString("ilwis://file/unknown.csy");
        if(!csy.prepare(res)) {
            kernel()->issues()->log(TR("Cound find coordinate system unknown, corrupt system file"));
            return false;
        }
    }

    grf->coordinateSystem(csy);
    double maxx = odf.value("GeoRefCorners","MaxX").toDouble();
    double maxy = odf.value("GeoRefCorners","MaxY").toDouble();
    double minx = odf.value("GeoRefCorners","MinX").toDouble();
    double miny = odf.value("GeoRefCorners","MinY").toDouble();
    if ( maxx == rUNDEF || maxy == rUNDEF || minx == rUNDEF || miny == rUNDEF) {
        kernel()->issues()->log(TR("Uninitialized boundaries for georeference %1").arg(_resource.name()));
        return false;
    }

    grf->setEnvelope(Box2D<double>(Coordinate(minx, miny), Coordinate(maxx, maxy)));
    return true;
}


IlwisObject *GeorefConnector::createGeoreference(const IniFile &odf) const{
    QString type = odf.value("GeoRef","Type");
    if ( type == "GeoRefCorners"){
        return new CornersGeoReference();
    }
    //TODO tiepoints georef
    if ( type == "GeoRefSubMap") {
        auto name = _odf->value("GeoRefSubMap","GeoRef");
        QUrl resource = mastercatalog()->name2url(name, itGEOREF);
        IniFile odf;
        odf.setIniFile(resource);
        return createGeoreference(odf);
    }
    return 0;
}

IlwisObject *GeorefConnector::create() const
{
    IniFile *odf = _odf.data();
    return createGeoreference(*odf);
}
