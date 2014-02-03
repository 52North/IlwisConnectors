#include <QtPlugin>
#include <QScopedPointer>
#include <functional>

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "geos/geom/Geometry.h"
#include "range.h"
#include "domain.h"
#include "datadefinition.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "columndefinition.h"
#include "table.h"
#include "catalog.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "attributerecord.h"
#include "feature.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "wfsmodule.h"
#include "wfsconnector.h"
#include "wfsfeatureconnector.h"
#include "catalogconnector.h"
#include "wfscatalogconnector.h"
#include "wfscontainerconnector.h"
#include "ilwisobjectfactory.h"
#include "wfsobjectfactory.h"
#include "wfsresponse.h"
#include "wfs.h"

using namespace Ilwis;
using namespace Wfs;

WfsModule::WfsModule(QObject *parent) :
    Module(parent, "WfsModule", "iv40","1.0")
{
}

WfsModule::~WfsModule()
{

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

    CatalogConnectorFactory *catfact = kernel()->factory<CatalogConnectorFactory>("ilwis::catalogconnectorfactory");
    if ( catfact) {
        catfact->add(WfsCatalogConnector::create);
    }

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (cfactory) {
        cfactory->addCreator(itFEATURE, "wfs", WfsFeatureConnector::create);
        cfactory->addCreator(itCONTAINER,"wfs", WfsContainerConnector::create);
    }


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

