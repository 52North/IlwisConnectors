#ifndef STREAMCOVERAGEDATAINTERFACEV1_H
#define STREAMCOVERAGEDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {


class StreamCoverageDataInterfaceV1 : public StreamConnectorV1
{
public:
    StreamCoverageDataInterfaceV1(QDataStream &stream);

    bool store(IlwisObject *obj, int options);
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
};
}
}

#endif // STREAMCOVERAGEDATAINTERFACEV1_H
