#ifndef STREAMGEOREFDATAINTERFACEV1_H
#define STREAMGEOREFDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {

class StreamGeorefDataInterfaceV1 : public StreamConnectorV1
{
public:
    StreamGeorefDataInterfaceV1(QDataStream &stream);

    bool store(IlwisObject *obj, int options);
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static DataInterface *create(QDataStream &stream);
};
}
}

#endif // STREAMGEOREFDATAINTERFACEV1_H
