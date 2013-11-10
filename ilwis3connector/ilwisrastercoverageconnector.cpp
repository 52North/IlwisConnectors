#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDir>
#include <fstream>
#include <iterator>

#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "table.h"
#include "module.h"
#include "numericrange.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "inifile.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "pixeliterator.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "rawconverter.h"
#include "coverageconnector.h"
#include "ilwisrastercoverageconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *RasterCoverageConnector::create(const Resource &resource, bool load) {
    return new RasterCoverageConnector(resource, load);

}



RasterCoverageConnector::RasterCoverageConnector(const Resource &resource, bool load) : CoverageConnector(resource, load),_storesize(1)
{
}

bool RasterCoverageConnector::loadMapList(IlwisObject *data) {
    Ilwis3Connector::loadMetaData(data);

    RasterCoverage *gcoverage = static_cast<RasterCoverage *>(data);

    QString file = filename2FullPath(_odf->value("MapList","Map0"));
    if ( file == sUNDEF)
        return ERROR2(ERR_COULD_NOT_LOAD_2,"RasterCoverage",_odf->file());

    IRasterCoverage mp;
    if (!mp.prepare("file:///" + file))
        return ERROR2(ERR_COULD_NOT_LOAD_2,"RasterCoverage",file);

    bool ok;
    qint32 z = _odf->value("MapList","Maps").toInt(&ok);
    Size sz = mp->size();
    sz.zsize(z);

    if (!ok || z < 0)
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Number of maps", gcoverage->name());

    for(int i = 0; i < z; ++i) {
        QString file = _odf->value("MapList",QString("Map%1").arg(i));
        //file = filename2FullPath(file);
        file = _resource.container().toString() + "/" + file;
        if ( file != sUNDEF) {
            IniFile odf;
            odf.setIniFile(file, containerConnector());
            //QString dataFile = filename2FullPath(odf.value("MapStore","Data"));
            QUrl url (_resource.container().toString() + "/" + odf.value("MapStore","Data"));
            _dataFiles.push_back(url);
        } else {
            ERROR2(ERR_COULD_NOT_LOAD_2,"files","maplist");
            --z;
        }
    }

    IniFile odf;
    if (!odf.setIniFile("file:///" + file, containerConnector()))
        return ERROR2(ERR_COULD_NOT_LOAD_2,"files","maplist");

    QString storeType = odf.value("MapStore","Type");
    setStoreType(storeType);

    gcoverage->datadef().domain(mp->datadef().domain());

    double vmax,vmin,scale,offset;
    QString range = odf.value("BaseMap","Range");
    if ( range != sUNDEF ) {
        if( getRawInfo(range, vmin,vmax,scale,offset)) {
            if ( scale == 1.0) {
                gcoverage->datadef().range(new NumericRange(vmin, vmax,1));
            }
            else {
                gcoverage->datadef().range(new NumericRange(vmin, vmax));
            }
        }
    }

    gcoverage->georeference(mp->georeference());
    gcoverage->size(sz);
    gcoverage->setCoordinateSystem(mp->coordinateSystem());
    gcoverage->envelope(mp->envelope());
    _dataType = mp->datadef().range()->determineType();

    return true;

}

void RasterCoverageConnector::setStoreType(const QString& storeType) {
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

bool RasterCoverageConnector::setDataDefinition(IlwisObject *data) {

    RasterCoverage *raster = static_cast<RasterCoverage *>(data);

    IDomain dom;
    if(!dom.prepare(_odf->file())) {
        kernel()->issues()->log(data->name(),TR(ERR_NO_INITIALIZED_1).arg(data->name()));
        return false;
    }

    raster->datadef() = DataDefinition(dom);
    double vmax,vmin,scale,offset;
    QString range = _odf->value("BaseMap","Range");
    if ( range != sUNDEF ) {
        if ( getRawInfo(range, vmin,vmax,scale,offset)) {
            if ( scale == 1.0) {
                raster->datadef().range(new NumericRange(vmin, vmax,1));

            }
            else {
                raster->datadef().range(new NumericRange(vmin, vmax));
            }


        }
    } else {
        QString dminfo = _odf->value("BaseMap","DomainInfo");
        if ( dminfo != sUNDEF) {
            int index = dminfo.indexOf("class;");
            if ( index != -1) {
                _converter = RawConverter("class");
            } else {
                index = dminfo.indexOf("id;");
                if ( index != -1) {
                    _converter = RawConverter("id");
                } else {
                    index = dminfo.indexOf("UniqueID;");
                    if ( index != -1) {
                        _converter = RawConverter("UniqueID");
                    }
                }
            }

        }
    }
    return true;
}

bool RasterCoverageConnector::loadMetaData(IlwisObject *data)
{
    Locker lock(_mutex);

    QFileInfo inf(_resource.toLocalFile());
    if(!setDataDefinition(data))
        return false;

    bool isMapList  = inf.suffix().toLower() == "mpl";

    if (isMapList ){
        return loadMapList(data);
    }
    else if(!CoverageConnector::loadMetaData(data))
        return false;

    RasterCoverage *gcoverage = static_cast<RasterCoverage *>(data);

    QString grfName = _odf->value("Map","GeoRef");
    grfName = filename2FullPath(grfName, gcoverage->source());
    IGeoReference grf;
    if (!grf.prepare(grfName)) {
        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Georeference",grfName));
        return false;
    }

    QString dataFile = filename2FullPath(_odf->value("MapStore","Data"));
    if ( dataFile != sUNDEF)
         _dataFiles.push_back(QUrl::fromLocalFile(dataFile));

    QString storeType = _odf->value("MapStore","Type");

    setStoreType(storeType);

    gcoverage->georeference(grf);
    _dataType = gcoverage->datadef().range()->determineType();

    return true;

}

IlwisObject *RasterCoverageConnector::create() const
{
    return new RasterCoverage(_resource);
}

inline double RasterCoverageConnector::value(char *block, int index) const{
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

qint64  RasterCoverageConnector::conversion(QFile& file, Grid *grid, int& count) {
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
            kernel()->issues()->log(TR("Reading past the end of file %1").arg(file.fileName()));
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

Grid* RasterCoverageConnector::loadGridData(IlwisObject* data)
{
    Locker lock(_mutex);

    if ( _dataFiles.size() == 0) {
        ERROR1(ERR_MISSING_DATA_FILE_1,_resource.name());
        return 0;
    }
    int blockCount = 0;
    RasterCoverage *raster = static_cast<RasterCoverage *>(data);
    Grid *grid = 0;
    if ( grid == 0) {
        Size sz = raster->size();
        grid =new Grid(sz);
    }
    grid->prepare();

    for(quint32 i=0; i < _dataFiles.size(); ++i) {
        QFileInfo localfile =  this->containerConnector()->toLocalFile(_dataFiles[i]);
        QString datafile = localfile.absoluteFilePath();
        if ( datafile.right(1) != "#") { // can happen, # is a special token in urls
            datafile += "#";
        }
        QFile file(datafile);
        if ( !file.exists()){
            ERROR1(ERR_MISSING_DATA_FILE_1,datafile);
            return 0;
        }
        if (!file.open(QIODevice::ReadOnly )) {
            ERROR1(ERR_COULD_NOT_OPEN_READING_1,datafile);
            return 0;
        }

        int result = conversion(file, grid, blockCount);

        file.close();
        if ( result == 0) {
            delete grid;
            return 0;
        }
    }
    if ( raster->attributeTable().isValid()) {
        ITable tbl = raster->attributeTable();
        IDomain covdom;
        if (!covdom.prepare("count")){
            return 0;
        }
        //tbl->addColumn(COVERAGEKEYCOLUMN,covdom);
        for(quint32 i=0; i < tbl->recordCount() ; ++i) {
            tbl->cell(COVERAGEKEYCOLUMN,i, QVariant(i));
        }
    }
    return grid;

}

bool RasterCoverageConnector::storeBinaryData(IlwisObject *obj)
{
    Locker lock(_mutex);

    if ( obj == nullptr)
        return false;
    IRasterCoverage raster = mastercatalog()->get(obj->id());
    if ( !raster.isValid())
        return false;

    if (!raster->georeference().isValid())
        return false;
    if ( raster->size().zsize() > 1) // mpl doesnt have binary data
        return true;

    const IDomain dom = raster->datadef().domain();
    if (!dom.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "Domain", raster->name());

    QFileInfo inf(obj->name());
    QString dir = context()->workingCatalog()->location().toLocalFile();
    QString filename = dir + "/" + inf.baseName() + ".mp#";
    Size sz = raster->size();
    bool ok = false;
    if ( dom->ilwisType() == itNUMERICDOMAIN) {
        calcStatics(obj, NumericStatistics::pBASIC);
        const NumericStatistics& stats = raster->statistics();
        RawConverter conv(stats[NumericStatistics::pMIN], stats[NumericStatistics::pMAX],pow(10, - stats.significantDigits()));


        std::ofstream output_file(filename.toLatin1(),ios_base::out | ios_base::binary | ios_base::trunc);
        if ( !output_file.is_open())
            return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,filename);

        if ( conv.storeType() == itUINT8) {
            ok = save<quint8>(output_file,conv.scale() == 1 ? RawConverter() : conv, raster,sz);
        } else if ( conv.storeType() == itINT16) {
            ok = save<qint16>(output_file,conv, raster,sz);
        } else if ( conv.storeType() == itINT32) {
            save<qint32>(output_file,conv, raster,sz);
        } else {
            ok = save<double>(output_file,conv, raster,sz);
        }
        output_file.close();

    } else if ( dom->ilwisType() == itITEMDOMAIN ){
        if ( hasType(dom->valueType(), itTHEMATICITEM | itNAMEDITEM)) {
            std::ofstream output_file(filename.toLatin1(),ios_base::out | ios_base::binary | ios_base::trunc);
            if ( !output_file.is_open()){
                return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,filename);
            }

            if( hasType(dom->valueType(), itTHEMATICITEM)){
                RawConverter conv("class");
                ok = save<quint8>(output_file,conv, raster,sz);
            }
            else{
                RawConverter conv("ident");
                ok = save<quint16>(output_file,conv, raster,sz);
            }
        }
    }
    return ok;

}

void RasterCoverageConnector::calcStatics(const IlwisObject *obj, NumericStatistics::PropertySets set) const {
    IRasterCoverage raster = mastercatalog()->get(obj->id());
    if ( !raster->statistics().isValid()) {
        PixelIterator iter(raster,Box3D<>(raster->size()));
        raster->statistics().calculate(iter, iter.end(),set);
    }
}

bool RasterCoverageConnector::storeMetaDataMapList(IlwisObject *obj) {
    bool ok = Ilwis3Connector::storeMetaData(obj, itRASTER);
    if ( !ok)
        return false;


    IRasterCoverage raster = mastercatalog()->get(obj->id());

    QString localName = getGrfName(raster);
    if ( localName == sUNDEF)
        return false;
    _odf->setKeyValue("Ilwis","Type","MapList");
    _odf->setKeyValue("MapList","GeoRef",localName);
    Size sz = raster->size();
    _odf->setKeyValue("MapList","Size",QString("%1 %2").arg(sz.ysize()).arg(sz.xsize()));
    _odf->setKeyValue("MapList","Maps",QString::number(sz.zsize()));

    for(int i = 0; i < sz.zsize(); ++i) {
        QString mapName = QString("%1_band_%2").arg(obj->name()).arg(i);
        _odf->setKeyValue("MapList",QString("Map%1").arg(i),mapName);

        Resource resource(itRASTER);
        resource.addProperty("size", IVARIANT(Size(sz.xsize(), sz.ysize())));
        resource.addProperty("bounds", IVARIANT(raster->envelope()));
        resource.addProperty("georeference", IVARIANT(raster->georeference()));
        resource.addProperty("coordinatesystem", IVARIANT(raster->coordinateSystem()));
        resource.addProperty("domain", IVARIANT(raster->datadef().domain()));
        mastercatalog()->addItems({resource});

        IRasterCoverage gcMap;
        gcMap.prepare(resource);
        gcMap->setName(mapName);
        gcMap->copyBinary(raster, i);
        //QUrl mapUrl = "file:///" + path + "/" + mapName + ".mpr";
        gcMap->connectTo(QUrl(), "map", "ilwis3", Ilwis::IlwisObject::cmOUTPUT);
        gcMap->store(IlwisObject::smBINARYDATA | IlwisObject::smMETADATA);
    }

    _odf->store("mpl", containerConnector());
    return true;
}

QString RasterCoverageConnector::getGrfName(const IRasterCoverage& raster) {
    const IGeoReference grf = raster->georeference();
    if (!grf.isValid()) {
        ERROR2(ERR_NO_INITIALIZED_2, "Georeference", raster->name());
        return sUNDEF;
    }
    QString localName = Resource::toLocalFile(grf->source().url(),false);
    QFileInfo localGrf(localName);
    if ( !localGrf.exists()) {
        //QFileInfo coveragePath(Resource::toLocalFile(obj->target()));
        localName = raster->name();
        int index;
        if ( (index = localName.indexOf(".")) != -1)
            localName = localName.left(index);
        localName += ".grf";
        grf->setName(localName);
        grf->connectTo(QUrl(), "georef", "ilwis3", Ilwis::IlwisObject::cmOUTPUT);
        grf->store(IlwisObject::smMETADATA);
    } else
        localName = localGrf.fileName();

    return localName;
}

bool RasterCoverageConnector::storeMetaData( IlwisObject *obj)  {
    Locker lock(_mutex);

    IRasterCoverage raster = mastercatalog()->get(obj->id());
    if (!raster.isValid())
        return false;
    if (!raster->georeference().isValid())
        return false;

    if ( raster->size().zsize() > 1)
        return storeMetaDataMapList(obj);

    bool ok = CoverageConnector::storeMetaData(obj, itRASTER, raster->datadef());
    if ( !ok)
        return false;

    _odf->setKeyValue("BaseMap","Type","Map");


    if ( !raster.isValid())
        return ERROR2(ERR_COULD_NOT_LOAD_2,"RasterCoverage", obj->name());

    QString localName = getGrfName(raster);
    if ( localName == sUNDEF)
        return false;

    _odf->setKeyValue("Map","GeoRef",localName);
    Size sz = raster->size();
    _odf->setKeyValue("Map","Size",QString("%1 %2").arg(sz.ysize()).arg(sz.xsize()));
    _odf->setKeyValue("Map","Type","MapStore");

    const IDomain dom = raster->datadef().domain();
    if ( dom->ilwisType() == itNUMERICDOMAIN) {
        const NumericStatistics& stats = raster->statistics();
        int digits = stats.significantDigits();
        RawConverter conv(stats[NumericStatistics::pMIN], stats[NumericStatistics::pMAX],pow(10, - digits));
        qint32 delta = stats[NumericStatistics::pDELTA];
        if ( delta >= 0 && delta < 256 &&  digits == 0){
           _odf->setKeyValue("MapStore","Type","Byte");
        } else if ( conv.storeType() == itUINT8){
           _odf->setKeyValue("MapStore","Type","Byte");
        } else if ( conv.storeType() == itINT16){
            _odf->setKeyValue("MapStore","Type","Int");
        } else if ( conv.storeType() == itINT32){
            _odf->setKeyValue("MapStore","Type","Long");
        } else if ( conv.storeType() == itDOUBLE){
            _odf->setKeyValue("MapStore","Type","Real");
        }
    } if ( hasType(dom->ilwisType(),itITEMDOMAIN)) {
        if ( hasType(dom->valueType(), itTHEMATICITEM))
            _odf->setKeyValue("MapStore","Type","Byte");
        else if ( hasType(dom->valueType(), itNAMEDITEM)) {
            _odf->setKeyValue("MapStore","Type","Int");
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

    _odf->store("mpr", containerConnector());


    return true;
}

