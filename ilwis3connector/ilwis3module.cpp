#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QFileInfo>
#include <QSize>
#include <fstream>

#include "kernel.h"
#include "raster.h"
#include "inifile.h"
#include "numericrange.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "factory.h"
#include "columndefinition.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "table.h"
#include "pixeliterator.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwis3module.h"
#include "ilwisobjectfactory.h"
#include "ilwis3objectfactory.h"
#include "catalog.h"
#include "catalogconnector.h"
#include "filecatalogconnector.h"
#include "ilwis3catalogconnector.h"
#include "catalogconnectorfactory.h"
#include "rawconverter.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "ellipsoidconnector.h"
#include "coordinatesystemconnector.h"
#include "georefconnector.h"
#include "coverageconnector.h"
#include "gridcoverageconnector.h"
#include "domainconnector.h"
#include "tableconnector.h"
#include "ilwis3projectionconnector.h"
#include "domainconnector.h"
#include "featureconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

Ilwis3Module::Ilwis3Module(QObject *parent) :
    Module(parent, "IlwisModule", "iv40","1.0")
{
}

QString Ilwis3Module::getInterfaceVersion() const
{
    return "iv40";

}

void Ilwis3Module::prepare()
{
    Ilwis3ObjectFactory *objfactory = new Ilwis3ObjectFactory();
    objfactory->prepare();
    kernel()->addFactory(objfactory );

    ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory)
        return ;
    factory->addCreator(itITEMDOMAIN | itNUMERICDOMAIN ,"ilwis3", Ilwis3::DomainConnector::create);
    factory->addCreator(itTABLE,"ilwis3",Ilwis3::TableConnector::create);
    factory->addCreator(itGRID,"ilwis3", Ilwis3::GridCoverageConnector::create);
    factory->addCreator(itGEOREF,"ilwis3", Ilwis3::GeorefConnector::create);
    factory->addCreator(itPROJECTION,"ilwis3", Ilwis3::ProjectionConnector::create);
    factory->addCreator(itELLIPSOID,"ilwis3",Ilwis3::EllipsoidConnector::create);
    factory->addCreator(itCOORDSYSTEM,"ilwis3",Ilwis3::CoordinateSystemConnector::create);
    factory->addCreator(itFEATURECOVERAGE,"ilwis3",Ilwis3::FeatureConnector::create);

    factory->addCreator("map","ilwis3",Ilwis3::GridCoverageConnector::create);
    factory->addCreator("georef","ilwis3",Ilwis3::GeorefConnector::create);
    factory->addCreator("polygonmap","ilwis3",Ilwis3::FeatureConnector::create);

    CatalogConnectorFactory *catfact = kernel()->factory<CatalogConnectorFactory>("catalogconnectorfactory", "ilwis");
    if ( catfact)
        catfact->add(Ilwis3CatalogConnector::create);

    IlwisObject::addTypeFunction(Ilwis3Connector::ilwisType);


}

QString Ilwis3Module::name() const
{
    return "Ilwis 3 connector plugin";
}

QString Ilwis3Module::version() const
{
    return "1.0";
}

void Ilwis3Module::getOperations(QVector<ICommandInfo *> &) const{
}





//Q_EXPORT_PLUGIN2(Ilwis3Connector, Ilwis::Ilwis3::Ilwis3Module)
