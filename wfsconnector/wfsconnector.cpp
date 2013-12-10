#include <QUrl>
#include <QString>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include "wfsConnector_global.h"

#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "wfsconnector.h"
#include "wfs.h"


using namespace Ilwis;
using namespace Wfs;


WfsConnector::WfsConnector(const Resource &resource, bool load) : IlwisObjectConnector(resource, load)
{
}


QString WfsConnector::provider() const
{
    return QString("wfs");
}

bool WfsConnector::loadMetaData(IlwisObject *data)
{
    WebFeatureService wfs(_resource.url());
    wfs.getCapabilities();

    return false;
}

bool WfsConnector::storeMetaData(const IlwisObject *obj, IlwisTypes type) const
{
    return false;
}

void WfsConnector::finishedSlot(QNetworkReply *reply)
{

}







