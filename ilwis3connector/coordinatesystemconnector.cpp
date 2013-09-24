#include <QUrl>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QStringList>

#include "kernel.h"
#include "geometries.h"
#include "inifile.h"
#include "connectorinterface.h"
#include "ilwisdata.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "coordinatesystemconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *CoordinateSystemConnector::create(const Resource &resource,bool load) {
    return new CoordinateSystemConnector(resource, load);

}

CoordinateSystemConnector::CoordinateSystemConnector(const Resource &resource, bool load) : Ilwis3Connector(resource, load)
{
    QString type = _odf->value("CoordSystem","Type");
}

IlwisObject *CoordinateSystemConnector::create() const {
    IlwisObject *object = 0;
    if ( type() == itCONVENTIONALCOORDSYSTEM)
        object = new ConventionalCoordinateSystem(_resource);
    return object;
}

bool CoordinateSystemConnector::loadMetaData(IlwisObject* data)
{
    Ilwis3Connector::loadMetaData(data);
    CoordinateSystem *csy = static_cast<CoordinateSystem *>(data);
    QString ellipsoideName;

    IEllipsoid ell = getEllipsoid();
    GeodeticDatum *datum = getDatum(ellipsoideName);
    if ( !ell.isValid() && ellipsoideName != sUNDEF){
        QString ellres = QString("ilwis://tables/ellipsoid?code=%1").arg(ellipsoideName);
       if (!ell.prepare(ellres)) {
              return ERROR1("No ellipsoid for this code %1",ellipsoideName);
       }
    }


    if ( type() == itCONVENTIONALCOORDSYSTEM ) {
        ConventionalCoordinateSystem *csycc = static_cast<ConventionalCoordinateSystem *>(csy);
        IProjection proj = getProjection();
        if ( !proj.isValid()) {
            return ERROR1(ERR_NO_INITIALIZED_1, "projection");
        }
        csycc->setDatum(datum);
        csycc->setEllipsoid(ell);
        csycc->setProjection(proj);
        proj->setCoordinateSystem(csycc);


        proj->setParameter(Projection::pvELLCODE, ell->toProj4());
        csycc->prepare();
    } else if ( type() == itUNKNOWN){
        //TODO other types of csy
    }
    return true;
}

bool CoordinateSystemConnector::canUse(const Resource& resource) // static
{
    IlwisTypes requiredType = resource.ilwisType();
    if ( (requiredType & itCOORDSYSTEM)!= 0)
        return true;
    QString file = resource.url().toLocalFile();
    QFileInfo inf(file);
    if ( inf.exists()) {
        IniFile odf;
        odf.setIniFile(inf.absoluteFilePath());
        QString t = odf.value("CoordSystem","Type");
        if ( t == sUNDEF ) {
            if ( odf.value("CoordSystem","Projection") != "") // necessary due to incompleteness of ilwis odf's
                t = "Projection";
        }
        if (t == "LatLon" && requiredType == itCONVENTIONALCOORDSYSTEM)
            return true;
        else if ( t == "Projection" && requiredType == itCONVENTIONALCOORDSYSTEM)
           return true;

    }
    return false;
}

IEllipsoid CoordinateSystemConnector::getEllipsoid() {
    QString ell = _odf->value("CoordSystem","Ellipsoid");
    if ( ell == "?")
        return IEllipsoid();
    if ( ell == "User Defined") {
        double invf = _odf->value("Ellipsoid","1/f").toDouble();
        double majoraxis = _odf->value("Ellipsoid","a").toDouble();
        Ellipsoid *ellips = new Ellipsoid();
        ellips->setEllipsoid(majoraxis,invf);;
        ellips->setName("User Defined");
        IEllipsoid ellipsoid(ellips);
        return ellipsoid;
    }
    IEllipsoid ellipsoid;
    QString code = name2Code(ell, "ellipsoid");
    if ( code == sUNDEF)
        return IEllipsoid();

    QString resource = QString("ilwis://tables/ellipsoid?code=%1").arg(code);

    ellipsoid.prepare(resource);

    return ellipsoid;
}

GeodeticDatum *CoordinateSystemConnector::getDatum(QString& ellipsoid) {
    QString datum =_odf->value("CoordSystem","Datum");
    if ( datum == sUNDEF)
        return 0; // not an error; simply no datum with this csy

    QString area = _odf->value("CoordSystem","Datum Area");
    if ( area != sUNDEF && area != "" )
        datum = datum + "." + area;
    QString code = name2Code(datum,"datum");

    if ( code == "?"){
        kernel()->issues()->log(TR("No datum code for this alias %1").arg(datum));
        return 0;
    }

    QSqlQuery stmt(kernel()->database());
    QString query = QString("Select * from datum where code='%1'").arg(code);

    if (stmt.exec(query)) {
        if ( stmt.next()) {
            GeodeticDatum *gdata = new GeodeticDatum();
            QString area = stmt.value(stmt.record().indexOf("area")).toString();
            QString code = stmt.value(stmt.record().indexOf("code")).toString();
            double dx = stmt.value(stmt.record().indexOf("dx")).toDouble();
            double dy = stmt.value(stmt.record().indexOf("dy")).toDouble();
            double dz = stmt.value(stmt.record().indexOf("dz")).toDouble();
            gdata->setArea(area);
            gdata->setCode(code);
            gdata->set3TransformationParameters(dx, dy, dz);
            ellipsoid = stmt.value(stmt.record().indexOf("ellipsoid")).toString();

            return gdata;

        } else {
            kernel()->issues()->log(TR("No datum for this code %1").arg(code));
        }
    } else {
        kernel()->issues()->logSql(stmt.lastError());
    }
    return 0;
}

IProjection CoordinateSystemConnector::getProjection() {
    QString projection = _odf->value("CoordSystem","Projection");
    if (projection == "?") {
        QString type = _odf->value("CoordSystem","Type");
        if ( type == "LatLon") {
            projection = type;
        } else
            return IProjection();
    }

    QString code = name2Code(projection, "projection");
    Resource resource(QUrl(QString("ilwis://tables/projection?code=%1").arg(code)), itPROJECTION);

    if ( code == sUNDEF) {
        kernel()->issues()->log(TR("Couldnt find projection %1").arg(projection));
        return IProjection();
    }

    IProjection proj;
    if(!proj.prepare(resource))
        return IProjection();

    bool ok;
    double falseEasting = _odf->value("Projection","False Easting").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvX0, falseEasting);
    double falseNorthing = _odf->value("Projection","False Northing").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvY0, falseNorthing);
    double centralMeridian = _odf->value("Projection","Central Meridian").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvLON0, centralMeridian);
    double centralParllel = _odf->value("Projection","Central Parallel").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvLAT0, centralParllel);
    double standardParllel = _odf->value("Projection","Standard Parallel 1").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvLAT1, standardParllel);
    standardParllel = _odf->value("Projection","Standard Parallel 2").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvLAT2, standardParllel);
    double lattitudeOfTrueScale = _odf->value("Projection","Latitude of True Scale").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvLATTS, lattitudeOfTrueScale);
    double scaleFactor = _odf->value("Projection","Scale Factor").toDouble(&ok);
    if ( ok)
        proj->setParameter(Projection::pvK0, scaleFactor);
    int gzone = _odf->value("Projection","Zone").toInt(&ok);
    if ( ok)
        proj->setParameter(Projection::pvZONE, gzone);
    QString hemisphere = _odf->value("Projection","Northern Hemisphere");
    if ( hemisphere == sUNDEF && code == "utm")
        proj->setParameter(Projection::pvNORTH, hemisphere);

    return proj;
}

QString CoordinateSystemConnector::createCsyFromCode(const QString& code) {
    if ( code == "epsg:4326")
        return  "LatlonWGS84.csy";
    //TODO, create csy based on proj4 data, epsg->proj4 can be done through the database
    return sUNDEF;


}
