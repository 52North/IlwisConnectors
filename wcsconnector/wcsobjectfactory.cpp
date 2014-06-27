#include <QSqlQuery>
#include <QObject>
#include <QStringList>
#include <QSqlError>
#include <QHash>
#include <fstream>

#include "kernel.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectconnector.h"
#include "wcsobjectfactory.h"
#include "wcsutils.h"


using namespace Ilwis;
using namespace Wcs;

WcsObjectFactory::WcsObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","Wcs","")
{
}

IlwisObject *WcsObjectFactory::create(const Resource &resource, const PrepareOptions &options) const
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    IlwisObjectConnector *connector = factory->createFromResource<IlwisObjectConnector>(resource, "wcs", options);

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
   IlwisObject *object = createObject(connector, options);
   if ( object)
       return object;

   delete connector;
   return 0;
}

bool WcsObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() == "ilwis")
        return false; // can't use anything marked as internal


    if (!WcsUtils::isValidWcsUrl(resource.url(true)))
        return false;

    return true;
}

