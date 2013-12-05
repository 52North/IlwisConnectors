#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "kernel.h"
#include "ilwisdata.h"
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
    QNetworkReply *reply =_networkManager->get(QNetworkRequest(_resource.url()));
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finishedSlot(QNetworkReply*)));

    return false;
}

bool WfsConnector::storeMetaData(const IlwisObject *obj, IlwisTypes type) const
{
    return false;
}







