#ifndef WFSSCHEMAINFO_H
#define WFSSCHEMAINFO_H

#include "wfsconnector_global.h"

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsSchemaInfo
{
public:
    WfsSchemaInfo();

    QString geometryAtttributeName() const;
    QMap<QString,QString> namespaceMappings() const;
    void setGeometryAtttributeName(QString geometryAtttributeName);
    void addNamespaceMapping(QString prefix, QString uriNamespace);

private:
    QString _geometryAttributeName;
    QMap<QString,QString> _namespaceMappings;
};
}
}

#endif // WFSSCHEMAINFO_H
