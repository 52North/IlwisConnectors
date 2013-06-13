#ifndef COVERAGECONNECTOR_H
#define COVERAGECONNECTOR_H

namespace Ilwis {
namespace Ilwis3{


class CoverageConnector : public Ilwis3Connector
{
public:
    CoverageConnector(const Resource& item, bool load=true);


protected:
    bool getRawInfo(const QString &range, double &vmin, double &vmax, double &scale, double &offset);
    virtual void calcStatics(const IlwisObject *obj,CoverageStatistics::PropertySets set) const = 0;
    bool loadMetaData(Ilwis::IlwisObject *data);
    bool storeMetaData(IlwisObject *obj) ;

    RawConverter _converter;
};
}
}

#endif // COVERAGECONNECTOR_H
