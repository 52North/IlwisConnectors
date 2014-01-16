#include "kernel.h"
#include "connectorinterface.h"
#include "catalog/containerconnector.h"
#include "gdalcontainerconnector.h"

namespace Ilwis{
    GdalContainerConnector::GdalContainerConnector(){
    }

    GdalContainerConnector::GdalContainerConnector(const Resource& resource):ContainerConnector(resource){
    }

    ConnectorInterface* GdalContainerConnector::create(const Resource& res, bool){
        return new GdalContainerConnector(res);
    }

    bool GdalContainerConnector::prepare(){
        return true;
    }

    std::vector<QUrl> GdalContainerConnector::sources(const QStringList &filter, int options) const{
        std::vector<QUrl> ret;
        ret.push_back(source().url());
        return ret;
    }

    QFileInfo GdalContainerConnector::toLocalFile(const QUrl &datasource) const{
        QFileInfo inf = datasource.toLocalFile();
        return inf;
    }

    bool GdalContainerConnector::isValid() const{
        return source().isValid();
    }

    QString GdalContainerConnector::provider() const{
        return "gdal";
    }

    bool GdalContainerConnector::canUse(const Resource &res) const{
        if (res.url().scheme() == "gdal")
            return true;
        return false;
    }
}
