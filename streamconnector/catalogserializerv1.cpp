#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "factory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "catalog.h"
#include "catalogserializerv1.h"

using namespace Ilwis;
using namespace Stream;

CatalogserializerV1::CatalogserializerV1(QDataStream &stream) : VersionedSerializer(stream)
{
}

bool CatalogserializerV1::loadItems(std::vector<Resource>& items ) {
    if (!VersionedSerializer::loadItems(items))
        return false;

    size_t sz;
    _stream >> sz;
    for(int i = 0; i < sz; ++i){
        Resource resource;
        resource.load(_stream);
        items.push_back(resource);
    }
    return true;
}

bool CatalogserializerV1::store(IlwisObject *obj, const Ilwis::IOOptions &options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;

    Catalog *catalog = static_cast<Catalog *>(obj);
    catalog->prepare();
    QString baseurl;
    if (!options.contains("baseurl"))
        return false;
    baseurl = options["baseurl"].toString();



    std::vector<Resource> items = catalog->items();
    _stream << items.size();

    for(Resource resource : items){
        QString sourceurl = resource.url(true).toString();
        QString tail = sourceurl.mid(sourceurl.lastIndexOf("/") + 1);
        QString url = QString(baseurl).arg(tail).arg(IlwisObject::type2Name(resource.ilwisType()));
        resource.setUrl(url);
        resource.setUrl(url, true);
        resource.store(_stream);

    }
    _stream << catalog->parentCatalog();

    return true;


}

bool CatalogserializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    std::vector<Resource> items;
    if(!loadItems(items))
        return false;

    mastercatalog()->addItems(items);

    QUrl url;
    _stream >> url;
    Catalog *catalog = static_cast<Catalog *>(obj);
    catalog->setParentCatalog(QUrl(url));

    return true;
}

VersionedSerializer *CatalogserializerV1::create(QDataStream &stream)
{
        return new CatalogserializerV1(stream);
}
