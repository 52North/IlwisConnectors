#ifndef STREAMELLIPSOIDDATAINTERFACEV1_H
#define STREAMELLIPSOIDDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {
class StreamEllipsoidDataInterfaceV1 : public StreamConnectorV1
{
public:
    StreamEllipsoidDataInterfaceV1(QDataStream &stream) ;

    bool store(IlwisObject *obj, int options);
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static DataInterface *create(QDataStream &stream);
};
}
}

#endif // STREAMELLIPSOIDDATAINTERFACEV1_H
