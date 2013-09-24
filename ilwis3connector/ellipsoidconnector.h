#ifndef ELLIPSOIDCONNECTOR_H
#define ELLIPSOIDCONNECTOR_H

namespace Ilwis {
namespace Ilwis3{

class EllipsoidConnector : public Ilwis3Connector
{
public:
    EllipsoidConnector(const Resource &resource, bool load=true);
    bool loadMetaData(IlwisObject *data);
    Ilwis::IlwisObject *create() const;
    static ConnectorInterface *create(const Resource &resource, bool load=true);
private:
    bool createEllipsoidFromCode(const QString &code, IlwisObject *data);
    bool createEllipsoidFromDatum(const QString &name, IlwisObject *data);
};
}
}

#endif // ELLIPSOIDCONNECTOR_H
