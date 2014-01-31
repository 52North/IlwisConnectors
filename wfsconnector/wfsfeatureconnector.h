#ifndef WFSFEATURECONNECTOR_H
#define WFSFEATURECONNECTOR_H

#include "wfsconnector_global.h"

namespace Ilwis{
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsFeatureConnector : public WfsConnector{
public:
    WfsFeatureConnector(const Ilwis::Resource &resource, bool load=true);

    bool loadMetaData(IlwisObject* data);
    bool loadBinaryData(IlwisObject* data);
    bool store(IlwisObject *obj, int);

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
    Ilwis::IlwisObject *create() const;
protected:

private:


};
}
}
#endif // WFSFEATURECONNECTOR_H


