#ifndef COVERAGECONNECTOR_H
#define COVERAGECONNECTOR_H

namespace Ilwis {
namespace Ilwis3{

class TableConnector;

class CoverageConnector : public Ilwis3Connector
{
public:
    CoverageConnector(const Resource& resource, bool load=true);



protected:
    bool getRawInfo(const QString &range, double &vmin, double &vmax, double &scale, double &offset) const;
    virtual void calcStatics(const IlwisObject *obj,NumericStatistics::PropertySets set) const = 0;
    bool loadMetaData(Ilwis::IlwisObject *data);
    bool storeMetaData(IlwisObject *obj, IlwisTypes type, const Ilwis::IDomain &dom) ;
    bool storeBinaryData(IlwisObject *obj, IlwisTypes tp);
    TableConnector *createTableStoreConnector(ITable &attTable, Coverage *coverage, IlwisTypes tp);

    RawConverter _converter;
    DataDefinition determineDataDefintion() const;
private:
    ITable prepareAttributeTable(const QString &file, const QString& basemaptype) const;
};
}
}

#endif // COVERAGECONNECTOR_H
