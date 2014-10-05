#ifndef CATALOGSERIALIZER_H
#define CATALOGSERIALIZER_H

namespace Ilwis {
namespace Stream {
class CatalogserializerV1 : public VersionedSerializer
{
public:
    CatalogserializerV1(QDataStream &stream) ;

    bool store(IlwisObject *obj, const Ilwis::IOOptions &options= IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    bool loadItems(std::vector<Resource> &items);
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // CATALOGSERIALIZER_H
