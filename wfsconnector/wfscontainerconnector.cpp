
#include <sstream>
#include <QString>

#include "wfsConnector_global.h"

#include "kernel.h"
#include "ilwis.h"
#include "connectorinterface.h"
#include "catalog/containerconnector.h"
#include "wfs.h"
#include "wfsresponse.h"
#include "wfscontainerconnector.h"

using namespace Ilwis;
using namespace Wfs;

WfsContainerConnector::WfsContainerConnector()
{
}

WfsContainerConnector::WfsContainerConnector(const Resource &resource):ContainerConnector(resource)
{

}

ConnectorInterface *WfsContainerConnector::create(const Resource &res, bool)
{
    return new WfsContainerConnector(res);
}

bool WfsContainerConnector::prepare()
{
    return true;
}

std::vector<QUrl> WfsContainerConnector::sources(const QStringList &filter, int options) const
{
    std::vector<QUrl> wfsFeatures;
    return wfsFeatures;
}

QFileInfo WfsContainerConnector::toLocalFile(const QUrl &datasource) const
{

    // TODO: what is the intent of this method?

    // TODO: Download resource here?
    QFileInfo inf = datasource.toLocalFile();
    return inf;
}

bool WfsContainerConnector::isValid() const
{
    return source().isValid();
}

QString WfsContainerConnector::provider() const
{
    return "wfs";
}

bool WfsContainerConnector::canUse(const Resource &resource) const
{
    QString scheme = resource.url().scheme();
    if ( scheme == "ilwis")
        return false; // can't use anything marked as internal

    if ( scheme != "http" && scheme != "https")
        return false; // can't read non http based data

    IlwisTypes type = resource.ilwisType() ;
    if ( type & itCONTAINER)
        return true;

    return false;
}

