#ifndef GDALCONTAINERCONNECTOR_H
#define GDALCONTAINERCONNECTOR_H

namespace Ilwis {
    class GdalContainerConnector : public ContainerConnector{
        public:
            GdalContainerConnector();
            GdalContainerConnector(const Resource& resource);
            virtual ~GdalContainerConnector() {}

            static ConnectorInterface *create(const Resource &res, bool);

            bool prepare();
            std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const;
            QFileInfo toLocalFile(const QUrl& datasource) const;
            bool isValid() const;
            QString provider() const;
            bool canUse(const Resource& res) const;
    };
}
#endif // GDALCONTAINERCONNECTOR_H
