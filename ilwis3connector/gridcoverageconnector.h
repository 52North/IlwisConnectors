#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis {
class BaseGrid;

namespace Ilwis3{

class GridCoverageConnector : public CoverageConnector
{
public:
    GridCoverageConnector(const Ilwis::Resource &item, bool load=true);

    bool loadMetaData(IlwisObject *data);
    bool storeMetaData(Ilwis::IlwisObject *obj);
    Ilwis::Grid *loadGridData(Ilwis::IlwisObject *) ;
    bool storeBinaryData(Ilwis::IlwisObject *obj);

    Ilwis::IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &item,bool load = true);

    void calcStatics(const IlwisObject *obj,CoverageStatistics::PropertySets set) const;

private:
    qint64 conversion(QFile &file, Ilwis::Grid *grid, int &count);
    //qint64 noconversionneeded(QFile &file, Ilwis::Grid *grid, int &count);
    double value(char *block, int index) const;
    void setStoreType(const QString &storeType);
    bool loadMapList(IlwisObject *data);
    bool storeMetaDataMapList(Ilwis::IlwisObject *obj);
    QString getGrfName(const IGridCoverage &gcov);

    template<typename T> bool save(std::ofstream& output_file,const RawConverter& conv, const IGridCoverage& gcov, const Size& sz) const{
        PixelIterator pixiter(gcov,Box3D<>(sz));
        for_each(pixiter, pixiter.end(), [&](double& v){
            T c = conv.real2raw(v);
            output_file.write(reinterpret_cast<const char*>(&c), sizeof(T));
       });
        return true;
    }

    vector<QFileInfo> _dataFiles;
    int _storesize;
    IlwisTypes _storetype;
    IlwisTypes _dataType;
};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
