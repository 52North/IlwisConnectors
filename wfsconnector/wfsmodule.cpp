#include <QtPlugin>
#include <QScopedPointer>
#include <functional>

#include "kernel.h"
#include "datadefinition.h"
#include "wfsconnector.h"
#include "polygon.h"
#include "geometry.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "wfsconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnector.h"
#include "wfscatalogconnector.h"
#include "ilwisobjectfactory.h"
#include "wfsobjectfactory.h"

using namespace Ilwis;
using namespace Wfs;

WfsModule::WfsModule(QObject *parent) :
    Module(parent, "WfsModule", "iv40","1.0")
{
}

WfsModule::~WfsModule()
{
    if ( gdal())
        delete gdal();
}

QString WfsModule::getInterfaceVersion() const
{
    return "iv40";

}

void WfsModule::prepare()
{
    WfsObjectFactory *factory = new WfsObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    CatalogConnectorFactory *catfact = kernel()->factory<CatalogConnectorFactory>("catalogconnectorfactory", "ilwis");
    if ( catfact)
        catfact->add(WfsCatalogConnector::create);

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;

    cfactory->addCreator(itFEATURE,"wfs", WfsConnector::create);
    IlwisObject::addTypeFunction(WfsConnector::ilwisType);
}

QString WfsModule::getName() const
{
    return "Wfs plugin";
}

QString WfsModule::getVersion() const
{
    return "1.0";
}

void WfsModule::getOperations(QVector<ICommandInfo *> &) const{
}

