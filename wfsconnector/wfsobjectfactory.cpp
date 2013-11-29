#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QHash>
#include <fstream>

#include "kernel.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectconnector.h"
#include "wfsobjectfactory.h"
#include "wfsconnector.h"

using namespace Ilwis;
using namespace Wfs;

WfsObjectFactory::WfsObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","wfs","")
{
}

IlwisObject *WfsObjectFactory::create(const Resource &resource) const
{
     const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
     WfsConnector *connector = factory->createFromResource<WfsConnector>(resource, "wfs");

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

bool WfsObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() == "ilwis")
        return false; // can't use anything marked as internal

    if ( resource.url().scheme() != "http" || resource.url().scheme() != "https")
        return false; // can't read non http based data

    QString filename = resource.url().toLocalFile();
    if ( filename == "")
        return false;

    IlwisTypes type = resource.ilwisType() ;
    if ( type & itFEATURE)
        return true;

    return false;
}

bool WfsObjectFactory::prepare()
{
    return false;
}
