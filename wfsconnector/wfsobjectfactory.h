#ifndef WFSOBJECTFACTORY_H
#define WFSOBJECTFACTORY_H

namespace Ilwis {

class WfsObjectFactory : public IlwisObjectFactory
{
public:
    WfsObjectFactory();

    IlwisObject *create(const Ilwis::Resource &resource) const;
    bool canUse(const Resource &resource) const;
    bool prepare();

};

}

#endif // WfsOBJECTFACTORY_H
