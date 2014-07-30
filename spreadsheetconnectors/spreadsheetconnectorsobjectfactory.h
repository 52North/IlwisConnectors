#ifndef SpreadSheetOBJECTFACTORY_H
#define SpreadSheetOBJECTFACTORY_H

namespace Ilwis {
namespace SpreadSheetConnectors {

class SpreadSheetConnectorsObjectFactory : public IlwisObjectFactory
{
public:
    SpreadSheetConnectorsObjectFactory();

    bool canUse(const Resource &resource) const;
    IlwisObject *create(const Resource &resource,const IOOptions& options=IOOptions()) const;
private:
    IlwisObject *createTable(const Resource &resource);
};
}
}

#endif // SpreadSheetOBJECTFACTORY_H
