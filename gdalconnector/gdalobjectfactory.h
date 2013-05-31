#ifndef GDALOBJECTFACTORY_H
#define GDALOBJECTFACTORY_H

namespace Ilwis {
namespace Gdal{
class GdalObjectFactory : public IlwisObjectFactory
{
public:
    GdalObjectFactory();
    bool canUse(const Resource &item) const;
    IlwisObject *create(const Resource &item) const;
};
}
}

#endif // GDALOBJECTFACTORY_H
