#ifndef SpreadSheetOBJECTFACTORY_H
#define SpreadSheetOBJECTFACTORY_H

namespace Ilwis {
namespace SpreadSheetConnectors {

class SpreadSheetConnectorsObjectFactory : public IlwisObjectFactory
{
public:
    SpreadSheetConnectorsObjectFactory();

    bool canUse(const Resource &resource) const;
    IlwisObject *create(const Resource &resource,const PrepareOptions& options=PrepareOptions()) const;
private:
    IlwisObject *createTable(const Resource &resource);
};
}
}

#endif // SpreadSheetOBJECTFACTORY_H
