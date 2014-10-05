#ifndef CATALOGSERIALIZER_H
#define CATALOGSERIALIZER_H

namespace Ilwis {
namespace Stream {
class Catalogserializer : public VersionedSerializer
{
public:
    Catalogserializer(QDataStream &stream) ;

    bool store(IlwisObject *obj, const Ilwis::IOOptions &options= IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // CATALOGSERIALIZER_H
