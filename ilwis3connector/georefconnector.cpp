#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QDir>

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "inifile.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "RawConverter.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "Eigen/LU"
#include "mathhelper.h"
#include "planarctpgeoreference.h"
#include "georefconnector.h"
#include "binaryilwis3table.h"
#include "factory.h"
#include "abstractfactory.h"
#include "georefimplementationfactory.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *GeorefConnector::create(const Resource &resource, bool load) {
    return new GeorefConnector(resource, load);

}


GeorefConnector::GeorefConnector(const Resource &resource, bool load) : Ilwis3Connector(resource, load) {

}


bool GeorefConnector::loadGeoref(const IniFile &odf, IlwisObject *data ) {
    QString type = odf.value("GeoRef","Type");
    if ( type == "?") {
        kernel()->issues()->log(TR("Invalid Georef section in %1").arg(odf.file()));
        return false;
    }
    bool ok1, ok2;
    quint32 lines = odf.value("GeoRef","Lines").toInt(&ok1);
    quint32 columns = odf.value("GeoRef","Columns").toInt(&ok2);
    if ( !(ok1 & ok2)) {
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Lines/Columns","Georeference");
    }

    GeoReference *grf = static_cast<GeoReference *>(data);
    grf->size(Size<>(columns, lines,1));
    if ( type == "GeoRefCorners") {
        return loadGeorefCorners(odf, data);
    } else if ( type == "GeoRefSubMap") {
        QString name = odf.value("GeoRefSubMap","GeoRef");
        QUrl resource = mastercatalog()->name2url(name, itGEOREF);
        IniFile odf;
        odf.setIniFile(resource, containerConnector());
        bool ok = loadGeoref(odf,data);
        if (!ok)
            return false;
        grf->size(Size<>(columns, lines,1)); // we dont want the size of the parent georef
        return true;
    } else if ( type == "GeoRefCTP"){
        return loadGeorefTiepoints(odf, grf);
    }
    return false;
}

bool GeorefConnector::loadGeorefTiepoints(const IniFile& odf, GeoReference *grf) {
    QString csyName = odf.value("GeoRef","CoordSystem");
    QUrl path = mastercatalog()->name2url(csyName, itCOORDSYSTEM);
    ICoordinateSystem csy;
    if(!csy.prepare(path.toLocalFile())) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, "coordinate system",csyName);
    }
    grf->coordinateSystem(csy);
    QSharedPointer<PlanarCTPGeoReference> grfctp(grf->impl<PlanarCTPGeoReference>());
    BinaryIlwis3Table tbl;
    tbl.load(_odf);
    std::vector<int> colindexes(10, iUNDEF);
    enum ColIndexID{ciiX=0, ciiY=1, ciiZ=2, ciiLAT=3, ciiLON=4, ciiXP=5, ciiYP=6, ciiACT=7, ciiDX=8, ciiDY=9, ciiNEVER=10};
    colindexes[ciiX] = tbl.index("X");
    colindexes[ciiY] = tbl.index("Y");
    colindexes[ciiZ] = tbl.index("Z");
    colindexes[ciiXP] = tbl.index("Col");
    colindexes[ciiYP] = tbl.index("Row");
    colindexes[ciiACT] = tbl.index("Active");
    //colindexes[ciiDX] = tbl.index("DCol");
    //colindexes[ciiDY] = tbl.index("DRow");
    for(int rec = 0; rec < tbl.rows(); ++rec){
        double x=rUNDEF;
        tbl.get(rec,colindexes[ciiX],x);
        double y=rUNDEF;
        tbl.get(rec,colindexes[ciiY],y);
            double z=0;
        if (colindexes[ciiZ] != iUNDEF) {
            tbl.get(rec,colindexes[ciiZ],z);
        }
        double xp=rUNDEF;
        tbl.get(rec,colindexes[ciiXP],xp);
        double yp=rUNDEF;
        tbl.get(rec,colindexes[ciiYP],yp);

        double active=1;
        tbl.get(rec,colindexes[ciiACT],active);
        ControlPoint ctp(Coordinate(x,y,z),Pixeld(xp,yp));
        ctp.active(active != 0);
        grfctp->setControlPoint(ctp);
    }
    QString transf = odf.value("GeoRefCTP", "Transformation");
    PlanarCTPGeoReference::Transformation transformation;
    if (transf == "Conform")
      transformation =  PlanarCTPGeoReference::tCONFORM;
    else if (transf == "Affine")
      transformation =  PlanarCTPGeoReference::tAFFINE;
    else if (transf == "SecondOrder")
      transformation =  PlanarCTPGeoReference::tSECONDORDER;
    else if (transf == "FullSecondOrder")
      transformation =  PlanarCTPGeoReference::tFULLSECONDORDER;
    else if (transf == "ThirdOrder")
      transformation =  PlanarCTPGeoReference::tTHIRDORDER;
    else if (transf == "Projective")
      transformation =  PlanarCTPGeoReference::tPROJECTIVE;
    grfctp->transformation(transformation);
    grfctp->compute();

    return true;
}

bool GeorefConnector::loadMetaData(IlwisObject *data)
{
    Ilwis3Connector::loadMetaData(data);
    IniFile *odf = _odf.data();

    return loadGeoref(*odf, data);
}

bool GeorefConnector::storeMetaData(IlwisObject *obj)
{
    Ilwis3Connector::storeMetaData(obj, itGEOREF);
    GeoReference *grf = static_cast<GeoReference *>(obj);
    _odf->setKeyValue("GeoRef","CoordSystem",Resource::toLocalFile(grf->coordinateSystem()->source().url(),true));
    Size<> sz = grf->size();
    _odf->setKeyValue("GeoRef","Lines", QString::number(sz.ysize()));
    _odf->setKeyValue("GeoRef","Columns", QString::number(sz.xsize()));
    //CornersGeoReference *cgrf = dynamic_cast<CornersGeoReference *>(grf);
    if ( grf->grfType<CornersGeoReference>()) {
        QSharedPointer<CornersGeoReference> cgrf = grf->impl<CornersGeoReference>();
        _odf->setKeyValue("GeoRef","CornersOfCorners", cgrf->isCornersOfCorners() ? "Yes" : "No");
        _odf->setKeyValue("GeoRef","Type", "GeoRefCorners");
        Coordinate cmin = cgrf->envelope().min_corner();
        Coordinate cmax = cgrf->envelope().max_corner();
        _odf->setKeyValue("GeoRefCorners", "MinX", QString::number(cmin.x));
        _odf->setKeyValue("GeoRefCorners", "MinY", QString::number(cmin.y));
        _odf->setKeyValue("GeoRefCorners", "MaxX", QString::number(cmax.x));
        _odf->setKeyValue("GeoRefCorners", "MaxY", QString::number(cmax.y));

        std::vector<double> matrix = cgrf->matrix();
        _odf->setKeyValue("GeoRefSmpl", "a11", QString::number(matrix[0]));
        _odf->setKeyValue("GeoRefSmpl", "a22", QString::number(matrix[3]));
        _odf->setKeyValue("GeoRefSmpl", "a12", "0");
        _odf->setKeyValue("GeoRefSmpl", "a21", "0");

         std::vector<double> support = cgrf->support();
         _odf->setKeyValue("GeoRefSmpl", "b1", QString::number(support[0]));
         _odf->setKeyValue("GeoRefSmpl", "b2", QString::number(support[1]));

         _odf->store("grf", containerConnector());
        return true;

    }

    return false;

}

bool GeorefConnector::loadGeorefCorners(const IniFile& odf, IlwisObject *data) {
    GeoReference *grf = static_cast<GeoReference *>(data);
    QString csyName = odf.value("GeoRef","CoordSystem");
    QUrl path = mastercatalog()->name2url(csyName, itCOORDSYSTEM);
    ICoordinateSystem csy;
    if(!csy.prepare(path.toLocalFile())) {
        kernel()->issues()->log(TR("Couldn't find coordinate system %1, loading unknown").arg(csyName),IssueObject::itWarning);
        QString resource = QString("ilwis://file/unknown.csy");
        if(!csy.prepare(resource)) {
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

    grf->impl<CornersGeoReference>()->setEnvelope(Envelope(Coordinate(minx, miny), Coordinate(maxx, maxy)));
    return true;
}


IlwisObject *GeorefConnector::createGeoreference(const IniFile &odf) const{
    QString type = odf.value("GeoRef","Type");
    if ( type == "GeoRefCorners"){
        return GeoReference::create("corners", _resource);

    }
    if ( type == "GeoRefCTP") {
        return GeoReference::create("tiepoints", _resource);
    }
    if ( type == "GeoRefSubMap") {
        auto name = _odf->value("GeoRefSubMap","GeoRef");
        QUrl resource = mastercatalog()->name2url(name, itGEOREF);
        IniFile odf;
        odf.setIniFile(resource, containerConnector());
        return createGeoreference(odf);
    }
    return 0;
}

IlwisObject *GeorefConnector::create() const
{
    IniFile *odf = _odf.data();
    return createGeoreference(*odf);
}
