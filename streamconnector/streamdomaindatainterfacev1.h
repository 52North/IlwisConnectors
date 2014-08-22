#ifndef STREAMDOMAINDATAINTERFACEV1_H
#define STREAMDOMAINDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {

class StreamDomainDataInterfaceV1 : public StreamConnectorV1
{
public:
    StreamDomainDataInterfaceV1(QDataStream& stream);

    bool store(IlwisObject *obj, int options);
    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    static DataInterface *create(QDataStream &stream);
};
}
}

#endif // STREAMDOMAINDATAINTERFACEV1_H
