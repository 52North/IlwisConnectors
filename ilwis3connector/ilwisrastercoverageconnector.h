#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis {
class BaseGrid;

namespace Ilwis3{

class RasterCoverageConnector : public CoverageConnector
{
public:
    RasterCoverageConnector(const Ilwis::Resource &resource, bool load=true);

    bool loadMetaData(IlwisObject *data);
    bool storeMetaData(Ilwis::IlwisObject *obj);
    Ilwis::Grid *loadGridData(Ilwis::IlwisObject *) ;
    bool storeBinaryData(Ilwis::IlwisObject *obj);

    Ilwis::IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource,bool load = true);

    void calcStatics(const IlwisObject *obj,NumericStatistics::PropertySets set) const;

private:
    qint64 conversion(QFile& file,Ilwis::Grid *grid, int &count);
    //qint64 noconversionneeded(QFile &file, Ilwis::Grid *grid, int &count);
    double value(char *block, int index) const;
    void setStoreType(const QString &storeType);
    bool loadMapList(IlwisObject *data);
    bool storeMetaDataMapList(Ilwis::IlwisObject *obj);
    QString getGrfName(const IRasterCoverage &raster);
    bool setDataType(IlwisObject *data);

    template<typename T> bool save(std::ofstream& output_file,const RawConverter& conv, const IRasterCoverage& raster, const Size& sz) const{
        PixelIterator pixiter(raster, BoundingBox(sz));
        int  i=0;
        T c = 0;
        const char *ptr = reinterpret_cast<const char*>(&c);
        //std::ofstream dum("d:/dummyout.txt",ios_base::out | ios_base::trunc);
        for_each(pixiter, pixiter.end(), [&](double& v){
            c = conv.real2raw(v);
             ++i;
            output_file.write(ptr, sizeof(T));
            //dum << v << " ";
       });
        return true;
    }

    vector<QUrl> _dataFiles;
    int _storesize;
    IlwisTypes _storetype;
    IlwisTypes _dataType;
};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
