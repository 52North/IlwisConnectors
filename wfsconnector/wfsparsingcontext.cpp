#include <QString>
#include <QMap>

#include "wfsparsingcontext.h"

using namespace Ilwis;
using namespace Wfs;

WfsParsingContext::WfsParsingContext()
{
}

QString WfsParsingContext::srsName() const
{
    return _srsName;
}

int WfsParsingContext::srsDimension() const
{
    return _srsDimension;
}

QString WfsParsingContext::geometryAtttributeName() const
{
    return _geometryAttributeName;
}

QMap<QString, QString> WfsParsingContext::namespaceMappings() const
{
    return _namespaceMappings;
}

void WfsParsingContext::setSrsName(QString srsName)
{
    _srsName = srsName;
}

void WfsParsingContext::setSrsDimension(int srsDimension)
{
    _srsDimension = srsDimension;
}

void WfsParsingContext::setGeometryAtttributeName(QString geometryAtttributeName)
{
    _geometryAttributeName = geometryAtttributeName;
}

void WfsParsingContext::addNamespaceMapping(QString prefix, QString uriNamespace)
{
    _namespaceMappings[prefix] = uriNamespace;
}



