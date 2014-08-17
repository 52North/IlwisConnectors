#include "kernel.h"
#include "version.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "factory.h"
#include "connectorinterface.h"
#include "streamconnectorv1.h"
#include "streamellipsoiddatainterfacev1.h"

using namespace Ilwis;
using namespace Stream;

DataInterface *StreamEllipsoidDataInterfaceV1::create(QDataStream& stream)
{
    return new StreamEllipsoidDataInterfaceV1(stream);
}

StreamEllipsoidDataInterfaceV1::StreamEllipsoidDataInterfaceV1(QDataStream& stream) : StreamConnectorV1(stream)
{
}

bool StreamEllipsoidDataInterfaceV1::store(IlwisObject *obj, int options)
{
    if (!StreamConnectorV1::store(obj, options))
        return false;
    Ellipsoid *ell = static_cast<Ellipsoid *>(obj);
    _stream << ell->majorAxis() << ell->flattening()<< ell->authority();

    return true;
}

bool StreamEllipsoidDataInterfaceV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!StreamConnectorV1::loadMetaData(obj, options))
        return false;
    double majorAxis, flattening;
    QString authority;
    _stream >> majorAxis >> flattening >> authority;
    Ellipsoid *ellipsoid = static_cast<Ellipsoid *>(obj);
    ellipsoid->setEllipsoid(majorAxis,flattening);
    ellipsoid->setAuthority(authority);


    return true;
}


