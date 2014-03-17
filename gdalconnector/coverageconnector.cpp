#include <QLibrary>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include "kernel.h"
#include "coverage.h"
#include "module.h"

#include "projection.h"
#include "ilwisdata.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "table.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "gdalproxy.h"
#include "gdalconnector.h"
#include "coverageconnector.h"



using namespace Ilwis;
using namespace Gdal;

CoverageConnector::CoverageConnector(const Resource& resource, bool load, const PrepareOptions &options) : GdalConnector(resource,load, options)
{
}

bool CoverageConnector::loadMetaData(Ilwis::IlwisObject *data){

    if(!GdalConnector::loadMetaData(data))
        return false;

    Coverage *coverage = static_cast<Coverage *>(data);
    if ( coverage == nullptr)
        return false;

    QFileInfo fileinf = containerConnector()->toLocalFile(source());
    ICoordinateSystem csy = setObject<ICoordinateSystem>("coordinatesystem", fileinf.absoluteFilePath());
    if(!csy.isValid()) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "coordinatesystem", coverage->name());
    }
    coverage->coordinateSystem(csy);

    return true;
}

bool CoverageConnector::store(IlwisObject *obj, IlwisTypes type)
{
    return GdalConnector::store(obj, type);
}

OGRSpatialReferenceH CoverageConnector::createSRS(const ICoordinateSystem& coordsystem) const{
    IConventionalCoordinateSystem csy = coordsystem.get<ConventionalCoordinateSystem>();
    QString proj4def = csy->projection()->toProj4();
    OGRSpatialReferenceH srsH = gdal()->newSpatialRef(0);
    OGRErr errOgr = gdal()->importFromProj4(srsH, proj4def.toLocal8Bit());
    if ( errOgr != OGRERR_NONE) {
        return 0;
    }
    return srsH;
}



