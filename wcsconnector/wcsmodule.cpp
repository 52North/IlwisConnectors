#include <QtPlugin>
#include <QScopedPointer>
#include <functional>
#include "kernel.h"

#include "kernel.h"
#include "raster.h"
#include "datadefinition.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
//#include "wcsconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "catalogconnectorfactory.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "wcsmodule.h"
#include "wcsobjectfactory.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "dataformat.h"

using namespace Ilwis;
using namespace Wcs;

WCSModule::WCSModule(QObject *parent) :
    Module(parent, "WCSModule", "iv40","1.0")
{
}

WCSModule::~WCSModule()
{
}

QString WCSModule::getInterfaceVersion() const
{
    return "iv40";

}

void WCSModule::prepare()
{
    WcsObjectFactory *factory = new WcsObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;

    cfactory->addCreator(itCATALOG,"wcs", CatalogConnector::create);

}

QString WCSModule::getName() const
{
    return "wcs plugin";
}

QString WCSModule::getVersion() const
{
    return "1.0";
}






