#ifndef PROJECTIONCONNECTOR_H
#define PROJECTIONCONNECTOR_H

namespace Ilwis {
namespace Ilwis3 {
class ProjectionConnector : public Ilwis3Connector
{
public:
    ProjectionConnector(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
    bool loadMetaData(IlwisObject *data);
    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
private:
    Projection::ProjectionParamValue mapKeyToEnum(const QString &key) const;
    QString _internalCode;
};
}
}

#endif // PROJECTIONCONNECTOR_H
