#ifndef COVERAGECONNECTOR_H
#define COVERAGECONNECTOR_H

namespace Ilwis {
namespace Ilwis3{

class TableConnector;

class CoverageConnector : public Ilwis3Connector
{
public:
    CoverageConnector(const Resource& resource, bool load=true,const PrepareOptions& options=PrepareOptions());



protected:
    bool getRawInfo(const QString &range, double &vmin, double &vmax, double &scale, double &offset) const;
    virtual void calcStatics(const IlwisObject *obj,NumericStatistics::PropertySets set) const = 0;
    bool loadMetaData(Ilwis::IlwisObject *data, const PrepareOptions &options);
    bool storeMetaData(IlwisObject *obj, IlwisTypes type, const Ilwis::IDomain &dom, const QString &baseName=sUNDEF) ;
   // bool storeBinaryData(IlwisObject *obj, IlwisTypes tp, const QString &baseName);
    TableConnector *createTableStoreConnector(ITable &attTable, Coverage *coverage, IlwisTypes tp, const QString &baseName);

    RawConverter _converter;
    DataDefinition determineDataDefintion(const Ilwis::ODF &odf, const PrepareOptions &) const;
protected:
    QString _domainName;
    QString _domainInfo;
    QString _csyName;
private:
    ITable prepareAttributeTable(const QString &file, const QString& basemaptype,const PrepareOptions &) const;

};
}
}

#endif // COVERAGECONNECTOR_H
