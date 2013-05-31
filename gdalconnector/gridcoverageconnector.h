#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis{
namespace Gdal{

class GridCoverageConnector : public CoverageConnector
{
public:

    GridCoverageConnector(const Ilwis::Resource &item, bool load=true);

    bool loadMetaData(IlwisObject *data);
    Ilwis::Grid *loadGridData(Ilwis::IlwisObject *data) ;

    static ConnectorInterface *create(const Ilwis::Resource &item, bool load=true);
    Ilwis::IlwisObject *create() const;
protected:
    double value(char *block, int index) const;
    int _layers;
    GDALDataType _gdalValueType;
    int _typeSize;

};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
