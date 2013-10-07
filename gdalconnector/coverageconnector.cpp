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
#include "table.h"
#include "ilwisobjectconnector.h"
#include "gdalproxy.h"
#include "gdalconnector.h"
#include "coverageconnector.h"



using namespace Ilwis;
using namespace Gdal;

CoverageConnector::CoverageConnector(const Resource& resource,bool load) : GdalConnector(resource,load)
{
}

bool CoverageConnector::loadMetaData(Ilwis::IlwisObject *data){

    Coverage *coverage = static_cast<Coverage *>(data);

    ICoordinateSystem csy = setObject<ICoordinateSystem>("coordinatesystem", _filename);
    if(!csy.isValid()) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "coordinatesystem", coverage->name());
    }
    coverage->setCoordinateSystem(csy);

    return true;
}

bool CoverageConnector::store(IlwisObject *obj, IlwisTypes type)
{
    return GdalConnector::store(obj, type);
}



