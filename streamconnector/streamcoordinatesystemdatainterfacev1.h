#ifndef STREAMCOORDINATESYSTEMCONNECTORV1_H
#define STREAMCOORDINATESYSTEMCONNECTORV1_H

namespace Ilwis {
namespace Stream {
class StreamCoordinateSystemDataInterfaceV1 : public StreamConnectorV1
{
public:
    StreamCoordinateSystemDataInterfaceV1(QDataStream& stream);

    bool store(IlwisObject *obj, int options);
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static DataInterface *create(QDataStream &stream);
};
}
}

#endif // STREAMCOORDINATESYSTEMCONNECTOR_H
