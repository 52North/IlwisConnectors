#ifndef FEATURECONNECTOR_H
#define FEATURECONNECTOR_H

namespace Ilwis {

class FeatureCoverage;

namespace Ilwis3{

class FeatureConnector : public CoverageConnector
{
public:
    FeatureConnector(const Ilwis::Resource &item, bool load);
    bool loadMetaData(Ilwis::IlwisObject *data);
    bool storeMetaData(IlwisObject *obj) ;
    bool loadBinaryData(Ilwis::IlwisObject *obj);

    static ConnectorInterface *create(const Resource &item, bool load);
    Ilwis::IlwisObject *create() const;
    void calcStatics(const IlwisObject *obj,CoverageStatistics::PropertySets set) const;
private:
    bool loadBinaryPoints(FeatureCoverage *fcoverage, Ilwis::ITable &tbl);
    bool loadBinarySegments(FeatureCoverage *fcoverage, ITable &tbl);
};
}
}

#endif // FEATURECONNECTOR_H
