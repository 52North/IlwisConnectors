#include <QUrl>
#include "kernel.h"
#include "catalog.h"
#include "ilwisdata.h"
#include "errorobject.h"
#include "ilwiscontext.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "wfsconnector.h"


using namespace Ilwis;
using namespace Wfs;


WfsConnector::WfsConnector(const Resource &resource, bool load) : IlwisObjectConnector(resource, load)
{

}


QString WfsConnector::provider() const {

}


bool WfsConnector::loadMetaData(IlwisObject *data)
{
    return false;
}

bool WfsConnector::storeMetaData(const IlwisObject *obj, IlwisTypes type) const
{
    return false;
}







