#ifndef COVERAGECONNECTOR_H
#define COVERAGECONNECTOR_H

namespace Ilwis {

class Coverage;

namespace Gdal {

class CoverageConnector : public GdalConnector
{
public:
    CoverageConnector(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
    bool loadMetaData(Ilwis::IlwisObject *data);

protected:
    bool store(IlwisObject *obj, IlwisTypes type);

    OGRSpatialReferenceH createSRS(const ICoordinateSystem &coordsystem) const;
private:
    //void setCsy(Coverage *coverage);
};
}
}

#endif // COVERAGECONNECTOR_H
