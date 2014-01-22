#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QHash>

#include "kernel.h"
#include "ilwiscontext.h"
#include "numericrange.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "domain.h"
#include "numericdomain.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "connectorfactory.h"
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"
#include "gdalobjectfactory.h"
#include "domainconnector.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "coordinatesystem.h"
#include "projection.h"
#include "georeference.h"
#include "conventionalcoordinatesystem.h"
#include "coordinatesystemconnector.h"
#include "coverageconnector.h"
#include "gdalobjectfactory.h"
#include "gdalproxy.h"

using namespace Ilwis;
using namespace Gdal;

GdalObjectFactory::GdalObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","gdal","")
{
}

IlwisObject *GdalObjectFactory::create(const Resource &resource) const
{

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    GdalConnector *connector = factory->createFromResource<GdalConnector>(resource, "gdal");

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
   IlwisObject *object = createObject(connector);
   if ( object)
       return object;

   delete connector;
   return 0;
}

bool GdalObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() == "ilwis") // can't use anything marked as internal
        return false;

    if (resource.url().scheme() != "gdal" && !gdal()->supports(resource))
        return false;

    IlwisTypes type = resource.ilwisType() ;
    if ( type & itDOMAIN)
        return true;
    else if ( type & itCOORDSYSTEM)
        return true;
    else if ( type & itRASTER)
        return true;
    else if ( type & itGEOREF)
        return true;
    else if ( type & itFEATURE)
        return true;
    else if ( type & itTABLE)
        return true;

    return false;
}
