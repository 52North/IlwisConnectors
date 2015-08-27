#ifndef WcsOBJECTFACTORY_H
#define WcsOBJECTFACTORY_H

namespace Ilwis {

namespace Wcs {

class WcsObjectFactory : public IlwisObjectFactory
{
public:
    WcsObjectFactory();
    IlwisObject *create(const Resource &resource,const IOOptions& options=IOOptions()) const;
    bool canUse(const Resource &resource) const;
};

}
}



#endif // WcsOBJECTFACTORY_H
