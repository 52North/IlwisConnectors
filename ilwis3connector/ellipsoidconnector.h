#ifndef ELLIPSOIDCONNECTOR_H
#define ELLIPSOIDCONNECTOR_H

namespace Ilwis {
namespace Ilwis3{

class EllipsoidConnector : public Ilwis3Connector
{
public:
    EllipsoidConnector(const Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
    bool loadMetaData(IlwisObject *data, const PrepareOptions &options);
    Ilwis::IlwisObject *create() const;
    static ConnectorInterface *create(const Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
private:
    bool createEllipsoidFromCode(const QString &code, IlwisObject *data);
    bool createEllipsoidFromDatum(const QString &name, IlwisObject *data);
};
}
}

#endif // ELLIPSOIDCONNECTOR_H
