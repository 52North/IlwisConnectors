#ifndef STREAMTABLEDATAINTERFACEV1_H
#define STREAMTABLEDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {

class StreamTableDataInterfaceV1 : public StreamConnectorV1
{
public:
    StreamTableDataInterfaceV1(QDataStream& stream);

    bool store(IlwisObject *obj, int options);
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static DataInterface *create(QDataStream &stream);
};
}
}

#endif // STREAMTABLEDATAINTERFACEV1_H
