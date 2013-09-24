#ifndef PROJECTIONCONNECTOR_H
#define PROJECTIONCONNECTOR_H

namespace Ilwis {
namespace Ilwis3 {
class ProjectionConnector : public Ilwis3Connector
{
public:
    ProjectionConnector(const Ilwis::Resource &resource, bool load=true);
    bool loadMetaData(IlwisObject *data);
    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
private:
    Projection::ProjectionParamValue mapKeyToEnum(const QString &key) const;
    QString _internalCode;
};
}
}

#endif // PROJECTIONCONNECTOR_H
