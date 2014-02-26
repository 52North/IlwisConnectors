#include <QString>
#include <QMap>

#include "wfsschemainfo.h"

using namespace Ilwis;
using namespace Wfs;

WfsSchemaInfo::WfsSchemaInfo()
{
}

QString WfsSchemaInfo::geometryAtttributeName() const
{
    return _geometryAttributeName;
}

QMap<QString, QString> WfsSchemaInfo::namespaceMappings() const
{
    return _namespaceMappings;
}

void WfsSchemaInfo::setGeometryAtttributeName(QString geometryAtttributeName)
{
    _geometryAttributeName = geometryAtttributeName;
}

void WfsSchemaInfo::addNamespaceMapping(QString prefix, QString uriNamespace)
{
    _namespaceMappings[prefix] = uriNamespace;
}



