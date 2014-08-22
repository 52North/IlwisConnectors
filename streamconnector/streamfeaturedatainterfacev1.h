#ifndef STREAMFEATUREDATAINTERFACEV1_H
#define STREAMFEATUREDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {


class StreamFeatureDataInterfaceV1 : public StreamCoverageDataInterfaceV1
{
public:
    StreamFeatureDataInterfaceV1(QDataStream& stream);

    bool store(IlwisObject *obj, int options);
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static DataInterface *create(QDataStream &stream);
};
}
}

#endif // STREAMFEATUREDATAINTERFACEV1_H
