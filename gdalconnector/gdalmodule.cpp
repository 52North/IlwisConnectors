#include <QtPlugin>
#include <QScopedPointer>
#include <functional>
#include "kernel.h"

#include "kernel.h"
#include "raster.h"
#include "geometries.h"
#include "range.h"
#include "datadefinition.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "columndefinition.h"
#include "table.h"
#include "catalog.h"
#include "pixeliterator.h"
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"
#include "coordinatesystemconnector.h"
#include "coverageconnector.h"
#include "gridcoverageconnector.h"
#include "polygon.h"
#include "geometry.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "gdalfeatureconnector.h"
#include "gdalfeaturetableconnector.h"
#include "domainconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "domainconnector.h"
#include "georefconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "gdalmodule.h"
#include "catalogconnector.h"
#include "gdalcatalogconnector.h"
#include "ilwisobjectfactory.h"
#include "gdalobjectfactory.h"
#include "gdalproxy.h"

using namespace Ilwis;
using namespace Gdal;

GdalModule::GdalModule(QObject *parent) :
    Module(parent, "GdalModule", "iv40","1.0")
{
}

GdalModule::~GdalModule()
{
    if ( gdal())
        delete gdal();
}

QString GdalModule::getInterfaceVersion() const
{
    return "iv40";

}

void GdalModule::prepare()
{
    GdalObjectFactory *factory = new GdalObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    CatalogConnectorFactory *catfact = kernel()->factory<CatalogConnectorFactory>("catalogconnectorfactory", "ilwis");
    if ( catfact)
        catfact->add(GdalCatalogConnector::create);

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;


    cfactory->addCreator(itITEMDOMAIN | itNUMERICDOMAIN ,"gdal", DomainConnector::create);
    cfactory->addCreator(itRASTER ,"gdal", RasterCoverageConnector::create);
    cfactory->addCreator(itGEOREF,"gdal", GeorefConnector::create);
    cfactory->addCreator(itCOORDSYSTEM,"gdal", CoordinateSystemConnector::create);
    cfactory->addCreator(itFEATURE,"gdal", GdalFeatureConnector::create);
    cfactory->addCreator(itFLATTABLE,"gdal", GdalFeatureTableConnector::create);

    for(int i=0; i < gdal()->getDriverCount(); ++i) {
        GDALDriverH driv = gdal()->getDriver(i);
        if ( driv) {
            QString shortName = gdal()->getShortName(driv);
            cfactory->addCreator(shortName,"gdal", RasterCoverageConnector::create);
        }
    }
    IlwisObject::addTypeFunction(GdalConnector::ilwisType);
}

QString GdalModule::getName() const
{
    return "Gdal plugin";
}

QString GdalModule::getVersion() const
{
    return "1.0";
}

void GdalModule::getOperations(QVector<ICommandInfo *> &) const{
}




