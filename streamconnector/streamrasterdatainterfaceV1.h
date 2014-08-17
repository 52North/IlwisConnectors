#ifndef STREAMRASTERDATAINTERFACE_H
#define STREAMRASTERDATAINTERFACE_H

namespace Ilwis {
namespace Stream {



class StreamRasterDataInterfaceV1 : public StreamCoverageDataInterfaceV1
{
public:
    StreamRasterDataInterfaceV1(QDataStream& stream);

    bool store(IlwisObject *obj, int options);
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static DataInterface *create(QDataStream &stream);

private:

};
}
}

#endif // STREAMRASTERDATAINTERFACE_H
