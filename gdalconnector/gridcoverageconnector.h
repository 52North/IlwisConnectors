#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis{
namespace Gdal{

class RasterCoverageConnector : public CoverageConnector
{
public:

    RasterCoverageConnector(const Ilwis::Resource &item, bool load=true);

    bool loadMetaData(IlwisObject *data);
    Ilwis::Grid *loadGridData(Ilwis::IlwisObject *data) ;

    static ConnectorInterface *create(const Ilwis::Resource &item, bool load=true);
    Ilwis::IlwisObject *create() const;
    bool store(IlwisObject *obj, int );

private:
    int _layers;
    GDALDataType _gdalValueType;
    int _typeSize;

    double value(char *block, int index) const;
    bool setGeotransform(RasterCoverage *rasterCoverage, GDALDatasetH dataset);

    template<typename DT> bool save(RasterCoverage *prasterCoverage, GDALDatasetH dataset,GDALDataType gdaltype){
        quint32 columns = prasterCoverage->size().xsize();
        IRasterCoverage rasterCoverage;
        rasterCoverage.set(prasterCoverage);
        PixelIterator iter(rasterCoverage);
        int bandcount = 1;
        std::vector<DT> data(columns);
        GDALRasterBandH hband = gdal()->getRasterBand(dataset,bandcount);
        if (!hband) {
            return ERROR1(ERR_NO_INITIALIZED_1,"raster band");
        }
        while(iter != iter.end()) {
            for_each(data.begin(), data.end(), [&](DT& v){
                v = *iter;
                ++iter;
            });
            gdal()->rasterIO(hband, GF_Write, 0, iter.position().y() - 1, columns, 1, (void *)&data[0],columns,1, gdaltype,0,0 );

            if ( iter.zchanged())     {
                hband = gdal()->getRasterBand(dataset,++bandcount);
                if (hband == 0)
                    break;
            }
        }
        return true;
    }

};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
