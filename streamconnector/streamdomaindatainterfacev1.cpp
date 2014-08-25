#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "streamconnectorv1.h"
#include "itemdomain.h"
#include "streamdomaindatainterfacev1.h"

using namespace Ilwis;
using namespace Stream;

DataInterface *StreamDomainDataInterfaceV1::create(QDataStream& stream)
{
    return new StreamDomainDataInterfaceV1(stream);
}

StreamDomainDataInterfaceV1::StreamDomainDataInterfaceV1(QDataStream &stream) : StreamConnectorV1(stream)
{
}

bool StreamDomainDataInterfaceV1::store(IlwisObject *obj, int options)
{
    Domain *dom = static_cast<Domain *>(obj);
    quint64 valueType = dom->valueType();
    _stream << valueType;

    if (!StreamConnectorV1::store(obj, options))
        return false;

    if ( dom->ilwisType() == itITEMDOMAIN){
        ItemDomain<DomainItem> *itemdom= static_cast<ItemDomain<DomainItem> *>(dom) ;
        _stream << itemdom->theme();

    }
    if ( dom->ilwisType() != itTEXTDOMAIN)
        dom->range()->store(_stream);

    return true;

}

bool StreamDomainDataInterfaceV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!StreamConnectorV1::loadMetaData(obj, options))
        return false;


    Domain *dom = static_cast<Domain *>(obj);
    if ( dom->ilwisType() == itITEMDOMAIN){
        QString theme;
        _stream >> theme;
        ItemDomain<DomainItem> *itemdom= static_cast<ItemDomain<DomainItem> *>(dom) ;
        itemdom->setTheme(theme);

    }
    if ( dom->ilwisType() != itTEXTDOMAIN){
        Range * range = Range::create(dom->valueType());
        if ( !range)
            return false;
        range->load(_stream);
    }

    return true;
}
