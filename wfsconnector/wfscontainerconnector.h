#ifndef WFSCONTAINERCONNECTOR_H
#define WFSCONTAINERCONNECTOR_H

#include "wfsconnector_global.h"

namespace Ilwis {
    class WfsContainerConnector: public ContainerConnector{
        public:
            WfsContainerConnector();
            WfsContainerConnector(const Resource& resource);
            virtual ~WfsContainerConnector() {}

            static ConnectorInterface *create(const Resource &res, bool);

            bool prepare();
            std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const;
            QFileInfo toLocalFile(const QUrl& datasource) const;
            bool isValid() const;
            QString provider() const;
            bool canUse(const Resource& resource) const;
    };

}

#endif // WFSCONTAINERCONNECTOR_H
