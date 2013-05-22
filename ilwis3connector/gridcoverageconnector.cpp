#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDir>
#include <fstream>
#include <iterator>

#include "kernel.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "connectorinterface.h"
#include "module.h"
#include "numericrange.h"
#include "inifile.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "coordinatesystem.h"
#include "valuedefiner.h"
#include "columndefinition.h"
#include "table.h"
#include "containerstatistics.h"
#include "coverage.h"
#include "georeference.h"
#include "grid.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "gridcoverage.h"
#include "pixeliterator.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "rawconverter.h"
#include "coverageconnector.h"
#include "gridcoverageconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *GridCoverageConnector::create(const Resource &item, bool load) {
    return new GridCoverageConnector(item, load);

}



GridCoverageConnector::GridCoverageConnector(const Resource &item, bool load) : CoverageConnector(item, load),_storesize(1)
{
}

bool GridCoverageConnector::loadMapList(IlwisObject *data) {
    Ilwis3Connector::loadMetaData(data);

    GridCoverage *gcoverage = static_cast<GridCoverage *>(data);

    QString file = filename2FullPath(_odf->value("MapList","Map0"));
    if ( file == sUNDEF)
        return ERROR2(ERR_COULD_NOT_LOAD_2,"GridCoverage",_odf->fileinfo().baseName());

    IGridCoverage mp;
    if (!mp.prepare("file:///" + file))
        return ERROR2(ERR_COULD_NOT_LOAD_2,"GridCoverage",file);

    bool ok;
    qint32 z = _odf->value("MapList","Maps").toInt(&ok);
    Size sz = mp->size();
    sz.zsize(z);

    if (!ok || z < 0)
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Number of maps", gcoverage->name());

    for(int i = 0; i < z; ++i) {
        QString file = _odf->value("MapList",QString("Map%1").arg(i));
        file = filename2FullPath(file);
        if ( file != sUNDEF) {
            IniFile odf;
            odf.setIniFile(file);
            QString dataFile = filename2FullPath(odf.value("MapStore","Data"));
            _dataFiles.push_back(dataFile);
        } else {
            ERROR2(ERR_COULD_NOT_LOAD_2,"files","maplist");
            --z;
        }
    }

    IniFile odf;
    if (!odf.setIniFile(file))
        return ERROR2(ERR_COULD_NOT_LOAD_2,"files","maplist");

    QString storeType = odf.value("MapStore","Type");
    setStoreType(storeType);

    gcoverage->setDomain(mp->domain());

    double vmax,vmin,scale,offset;
    QString range = odf.value("BaseMap","Range");
    if ( range != sUNDEF ) {
        if( getRawInfo(range, vmin,vmax,scale,offset)) {
            if ( scale == 1.0) {
                gcoverage->setRange(new NumericRange(vmin, vmax,1));
            }
            else {
                gcoverage->setRange(new NumericRange(vmin, vmax));
            }
        }
    }

    gcoverage->setGeoreference(mp->georeference());
    gcoverage->size(sz);
    gcoverage->setCoordinateSystem(mp->coordinateSystem());
    gcoverage->setEnvelope(mp->envelope());
    _dataType = mp->range()->determineType();

    return true;

}

void GridCoverageConnector::setStoreType(const QString& storeType) {
    _storetype = itUINT8;
    if ( storeType == "Int") {
        _storesize = 2;
        _storetype = itINT16;
    }
    else if ( storeType == "Long"){
        _storesize = 4;
        _storetype = itINT32;
    }
    else if ( storeType == "Float"){
        _storesize = 4;
        _storetype = itFLOAT;
    }
    else if ( storeType == "Real") {
        _storesize = 8;
        _storetype = itDOUBLE;
    }
    _converter.storeType(_storetype);
}

bool GridCoverageConnector::loadMetaData(IlwisObject *data)
{
    Locker lock(_mutex);

    QFileInfo inf(_resource.toLocalFile());
    bool isMapList  = inf.suffix().toLower() == "mpl";

    if (isMapList ){
        return loadMapList(data);
    }
    else if(!CoverageConnector::loadMetaData(data))
        return false;

    GridCoverage *gcoverage = static_cast<GridCoverage *>(data);

    QString grfName = _odf->value("Map","GeoRef");
    IGeoReference grf;
    if (!grf.prepare(grfName)) {
        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Georeference",grfName));
        return false;
    }

    QString dataFile = filename2FullPath(_odf->value("MapStore","Data"));
    if ( dataFile != sUNDEF)
         _dataFiles.push_back(dataFile);

    QString storeType = _odf->value("MapStore","Type");

    setStoreType(storeType);

    gcoverage->setGeoreference(grf);
    _dataType = gcoverage->range()->determineType();

    return true;

}

IlwisObject *GridCoverageConnector::create() const
{
    return new GridCoverage(_resource);
}

inline double GridCoverageConnector::value(char *block, int index) const{
    double v = rUNDEF;
    char *c = &(block[index * _storesize]);
    switch (_storetype) {
    case itUINT8:
        v = *(quint8 *)c; break;
    case itINT16:
        v =  *(qint16 *)c; break;
    case itINT32:
        v = *(qint32 *)c; break;
    case itFLOAT:
        v = *(float *)c; break;
    case itINT64:
        return *(qint64 *)c;
    case itDOUBLE:
        v = *(double *)c; break;
    }
    return v;
}

qint64  GridCoverageConnector::conversion(QFile& file, Grid *grid, int& count) {
    qint64 blockSizeBytes = grid->blockSize(0) * _storesize;
    qint64 szLeft = grid->size().xsize() * grid->size().ysize() * _storesize;
    qint64 result = 0;
    qint64 totalRead =0;
    char *block = new char[blockSizeBytes];
    bool noconversionneeded = _converter.isNeutral();
    while(szLeft > 0) {
        if ( szLeft >= blockSizeBytes)
            result = file.read((char *)block,blockSizeBytes);
        else {
            result = file.read((char *)block,szLeft);
        }
        if ( result == -1){
            kernel()->issues()->log(TR("Reading past the end of file %1").arg(_dataFiles[0].fileName()));
            break;
        }
        quint32 noItems = grid->blockSize(count);
         if ( noItems == iUNDEF)
            return 0;
        vector<double> values(noItems);
        for(quint32 i=0; i < noItems; ++i) {
            double v = value(block, i);

            values[i] = noconversionneeded ? v :_converter.raw2real(v);
        }
        grid->setBlock(count, values, true);
        totalRead += result;
        ++count;
        szLeft -= blockSizeBytes;

    }
    delete [] block;

    return totalRead;
}

Grid* GridCoverageConnector::loadGridData(IlwisObject* data)
{
    Locker lock(_mutex);

    if ( _dataFiles.size() == 0) {
        ERROR1(ERR_MISSING_DATA_FILE_1,_resource.name());
        return 0;
    }
    int blockCount = 0;
    Grid *grid = 0;
    if ( grid == 0) {
        GridCoverage *gc = static_cast<GridCoverage *>(data);
        Size sz = gc->size();
        grid =new Grid(sz);
    }
    grid->prepare();

    for(quint32 i=0; i < _dataFiles.size(); ++i) {
        QFile file(_dataFiles[i].absoluteFilePath());
        if ( !file.exists()){
            ERROR1(ERR_MISSING_DATA_FILE_1,_dataFiles[0].fileName());
            return 0;
        }
        if (!file.open(QIODevice::ReadOnly )) {
            ERROR1(ERR_COULD_NOT_OPEN_READING_1,_dataFiles[0].fileName());
            return 0;
        }

        int result = conversion(file, grid, blockCount);

        file.close();
        if ( result == 0) {
            delete grid;
            return 0;
        }
    }
    return grid;

}

bool GridCoverageConnector::storeBinaryData(IlwisObject *obj)
{
    Locker lock(_mutex);

    if ( obj == nullptr)
        return false;
    IGridCoverage gcov = mastercatalog()->get(obj->id());
    if ( gcov->size().zsize() > 1) // mpl doesnt have binary data
        return true;

    const IDomain dom = gcov->domain();
    if (!dom.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "Domain", gcov->name());

    QFileInfo inf(_resource.toLocalFile());
    QString dir = context()->workingCatalog()->location().toLocalFile();
    QString filename = dir + "/" + inf.baseName() + ".mp#";
    bool ok = false;
    if ( dom->ilwisType() == itNUMERICDOMAIN) {
        calcStatics(obj, CoverageStatistics::pBASIC);
        RawConverter conv(gcov->statistics().min(), gcov->statistics().max(),pow(10, -gcov->statistics().significantDigits()));
        Size sz = gcov->size();

        std::ofstream output_file(filename.toLatin1(),ios_base::out | ios_base::binary | ios_base::trunc);
        if ( !output_file.is_open())
            return false;

        if ( conv.storeType() == itUINT8) {
            ok = save<quint8>(output_file,RawConverter(), gcov,sz);
        } else if ( conv.storeType() == itINT16) {
            ok = save<qint16>(output_file,conv, gcov,sz);
        } else if ( conv.storeType() == itINT32) {
            save<qint32>(output_file,conv, gcov,sz);
        } else {
            ok = save<double>(output_file,conv, gcov,sz);
        }
        output_file.close();

    }
    return ok;

}

void GridCoverageConnector::calcStatics(const IlwisObject *obj, CoverageStatistics::PropertySets set) const {
    IGridCoverage gcov = mastercatalog()->get(obj->id());
    if ( !gcov->statistics().isValid()) {
        PixelIterator iter(gcov,Box3D<>(gcov->size()));
        gcov->statistics().calculate(iter, iter.end(),set);
    }
}

bool GridCoverageConnector::storeMetaDataMapList(IlwisObject *obj) {
    bool ok = Ilwis3Connector::storeMetaData(obj);
    if ( !ok)
        return false;


    IGridCoverage gcov = mastercatalog()->get(obj->id());

    QString localName = getGrfName(gcov);
    if ( localName == sUNDEF)
        return false;
    _odf->setKeyValue("Ilwis","Type","MapList");
    _odf->setKeyValue("MapList","GeoRef",localName);
    Size sz = gcov->size();
    _odf->setKeyValue("MapList","Size",QString("%1 %2").arg(sz.ysize()).arg(sz.xsize()));
    _odf->setKeyValue("MapList","Maps",QString::number(sz.zsize()));

    for(int i = 0; i < sz.zsize(); ++i) {
        QString mapName = QString("%1_band_%2").arg(obj->name()).arg(i);
        _odf->setKeyValue("MapList",QString("Map%1").arg(i),mapName);

        Resource res(itGRIDCOVERAGE);
        res.addProperty("size", IVARIANT(sz));
        res.addProperty("bounds", IVARIANT(gcov->envelope()));
        res.addProperty("georeference", IVARIANT(gcov->georeference()));
        res.addProperty("coordinatesystem", IVARIANT(gcov->coordinateSystem()));
        res.addProperty("domain", IVARIANT(gcov->domain()));
        mastercatalog()->addItems({res});

        IGridCoverage gcMap;
        gcMap.prepare(res);
        gcMap->setName(mapName);
        gcMap->copyBinary(gcov, i);
        //QUrl mapUrl = "file:///" + path + "/" + mapName + ".mpr";
        gcMap->connectTo(QUrl(), "map", "ilwis3", Ilwis::IlwisObject::cmOUTPUT);
        gcMap->store(IlwisObject::smBINARYDATA | IlwisObject::smMETADATA);
    }

    _odf->store("mpl");
    return true;
}

QString GridCoverageConnector::getGrfName(const IGridCoverage& gcov) {
    const IGeoReference grf = gcov->georeference();
    if (!grf.isValid()) {
        ERROR2(ERR_NO_INITIALIZED_2, "Georeference", gcov->name());
        return sUNDEF;
    }
    QString localName = Resource::toLocalFile(grf->source(),true);
    QFileInfo localGrf(localName);
    if ( !localGrf.exists()) {
        //QFileInfo coveragePath(Resource::toLocalFile(obj->target()));
        localName = gcov->name() + ".grf";
        grf->setName(localName);
        grf->connectTo(QUrl(), "georef", "ilwis3", Ilwis::IlwisObject::cmOUTPUT);
        grf->store(IlwisObject::smMETADATA);
    } else
        localName = localGrf.fileName();

    return localName;
}

bool GridCoverageConnector::storeMetaData( IlwisObject *obj)  {
    Locker lock(_mutex);

    IGridCoverage gcov = mastercatalog()->get(obj->id());
    if (!gcov.isValid())
        return false;
    if ( gcov->size().zsize() > 1)
        return storeMetaDataMapList(obj);

    bool ok = CoverageConnector::storeMetaData(obj);
    if ( !ok)
        return false;

    _odf->setKeyValue("BaseMap","Type","Map");


    if ( !gcov.isValid())
        return ERROR2(ERR_COULD_NOT_LOAD_2,"GridCoverage", obj->name());

    QString localName = getGrfName(gcov);
    if ( localName == sUNDEF)
        return false;

    _odf->setKeyValue("Map","GeoRef",localName);
    Size sz = gcov->size();
    _odf->setKeyValue("Map","Size",QString("%1 %2").arg(sz.ysize()).arg(sz.xsize()));
    _odf->setKeyValue("Map","Type","MapStore");

    const IDomain dom = gcov->domain();
    if ( dom->ilwisType() == itNUMERICDOMAIN) {
        int digits = gcov->statistics().significantDigits();
        RawConverter conv(gcov->statistics().min(), gcov->statistics().max(),pow(10, -digits)) ;
        qint32 delta = gcov->statistics().max() - gcov->statistics().min();
        if ( delta >= 0 && delta < 256 &&  digits == 0){
           _odf->setKeyValue("MapStore","Type","Byte");
        }
        else if ( conv.storeType() == itINT16){
            _odf->setKeyValue("MapStore","Type","Int");
        } else if ( conv.storeType() == itINT32){
            _odf->setKeyValue("MapStore","Type","Long");
        } else if ( conv.storeType() == itDOUBLE){
            _odf->setKeyValue("MapStore","Type","Real");
        }
    }

    QFileInfo inf(_resource.toLocalFile());
    QString file = inf.baseName() + ".mp#";
    _odf->setKeyValue("MapStore","Data",file);
    _odf->setKeyValue("MapStore","Structure","Line");
    _odf->setKeyValue("MapStore","StartOffset","0");
    _odf->setKeyValue("MapStore","RowLength",QString::number(sz.xsize()));
    _odf->setKeyValue("MapStore","PixelInterLeaved","No");
    _odf->setKeyValue("MapStore","SwapBytes","No");
    _odf->setKeyValue("MapStore","UseAs","No");

    _odf->store();


    return true;
}

