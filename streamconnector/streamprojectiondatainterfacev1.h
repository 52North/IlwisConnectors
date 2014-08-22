#ifndef STREAMPROJECTIONDATAINTERFACEV1_H
#define STREAMPROJECTIONDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {

class StreamProjectionDataInterfaceV1 : public StreamConnectorV1
{
public:
    StreamProjectionDataInterfaceV1(QDataStream& stream);

    bool store(IlwisObject *obj, int options);
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static DataInterface *create(QDataStream &stream);
};
}
}

#endif // STREAMPROJECTIONDATAINTERFACEV1_H
