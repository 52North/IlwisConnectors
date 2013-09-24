#ifndef COVERAGECONNECTOR_H
#define COVERAGECONNECTOR_H

namespace Ilwis {

class Coverage;

namespace Gdal {

class CoverageConnector : public GdalConnector
{
public:
    CoverageConnector(const Ilwis::Resource &resource, bool load=true);
    bool loadMetaData(Ilwis::IlwisObject *data);

protected:
    bool store(IlwisObject *obj, IlwisTypes type);
    bool setSRS(Coverage *raster, GDALDatasetH dataset) const;
private:
    //void setCsy(Coverage *coverage);
};
}
}

#endif // COVERAGECONNECTOR_H
