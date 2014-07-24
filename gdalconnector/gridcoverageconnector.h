#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis{
namespace Gdal{

class RasterCoverageConnector : public CoverageConnector
{
public:

    RasterCoverageConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool loadData(Ilwis::IlwisObject *data, const IOOptions& options = IOOptions()) ;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    Ilwis::IlwisObject *create() const;
    bool store(IlwisObject *obj, int );

    bool setSRS(Coverage *raster, GDALDatasetH dataset) const;
    void reportError(GDALDatasetH dataset) const;

private:
    int _layers;
    GDALDataType _gdalValueType;
    int _typeSize;
    GDALDriverH _driver;

    double value(char *block, int index) const;
    bool setGeotransform(RasterCoverage *raster, GDALDatasetH dataset);

    template<typename DT> bool save(RasterCoverage *prasterCoverage, GDALDatasetH dataset,GDALDataType gdaltype){
        quint32 columns = prasterCoverage->size().xsize();
        IRasterCoverage raster;
        raster.set(prasterCoverage);
        PixelIterator iter(raster);
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

            double y = iter.zchanged() ? iter.box().ylength()  : iter.position().y;

            if(iter == iter.end()){
                y = iter.box().ylength();
            }

            gdal()->rasterIO(hband, GF_Write, 0, y - 1, columns, 1, (void *)&data[0],columns,1, gdaltype,0,0 );

            if ( iter.zchanged())     {
                hband = gdal()->getRasterBand(dataset,++bandcount);
                if (hband == 0)
                    break;
            }
        }
        return true;
    }

    bool loadDriver();
    DataDefinition createDataDef(double vmin, double vmax, double resolution);
    void loadBlock(GDALRasterBandH bandhandle, quint32 index, quint32 gdalindex, quint32 linesPerBlock, quint64 linesLeft, char *block, Ilwis::UPGrid &grid) const;
};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
