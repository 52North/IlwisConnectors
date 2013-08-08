#include <QString>
#include <QFile>

#include "kernel.h"
#include "angle.h"
#include "point.h"
#include "inifile.h"
#include "ilwisdata.h"
#include "domain.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "domain.h"
#include "datadefinition.h"
#include "numericrange.h"
#include "RawConverter.h"
#include "binaryilwis3table.h"


using namespace Ilwis ;
using namespace Ilwis3;

BinaryIlwis3Table::BinaryIlwis3Table() : _rows(0), _columns(0),_recordSize(0),_records(0)
{
}

BinaryIlwis3Table::~BinaryIlwis3Table(){
    for(quint32 r = 0; r < _rows; ++r) {
        for(quint32 c = 0; c < _columns; ++c) {
            const ColumnInfo& info = _columnInfo.at(c);
            char *p = _records + r * _recordSize + info._offset;
            if ( info._type == itBINARY){ //binaries are coords in this case
                vector<Coordinate> *coords = ( vector<Coordinate> *)(*(long *)p);
                delete coords;
            }
            else if	( info._type == itSTRING) {
                //delete p;
            }
        }
    }
    delete [] _records;
}

bool BinaryIlwis3Table::load(const ODF& odf, const QString& prfix){
    QString prefix = prfix == "" ? "" : prfix + ":";

    bool ok;
    _columns = odf->value(prefix + "Table","Columns").toLong(&ok);
    if (!ok) {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("column",odf->fileinfo().baseName()));
        return false;
    }
    _rows = odf->value(prefix + "Table","Records").toLong(&ok);
    if (!ok) {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("records",odf->fileinfo().baseName()));
        return false;
    }
    QString datafile = odf->value(prefix + "TableStore", "Data");
    datafile = context()->workingCatalog()->filesystemLocation().toLocalFile() + "/" + datafile;
    QFile file(datafile);

    if (!file.exists()){
        kernel()->issues()->log(TR(ERR_MISSING_DATA_FILE_1).arg(file.fileName()));
        return false;
    }
    if(!file.open(QIODevice::ReadOnly )){
        kernel()->issues()->log(TR(ERR_COULD_NOT_OPEN_READING_1).arg(file.fileName()));
        return false;
    }

    getColumnInfo(odf, prefix);

    qint64 size = file.size();
    char *memblock = new char [size];
    file.seek (0);
    qint64 r = file.read (memblock, size);
    file.close();

    _records = new char [ _recordSize * _rows];
    memset(_records, 0,_recordSize * _rows);

    readData(memblock);

    delete[] memblock;

    return true;
}

void BinaryIlwis3Table::getColumnInfo(const ODF& odf, const QString& prefix) {
    _columnInfo.resize(_columns);
    _recordSize = 0;

    for(quint32 col = 0 ; col < _columns; ++col) {
        ColumnInfo inf;
        QString key = QString("Col%1").arg(col);
        QString name = odf->value(prefix + "TableStore",key);
        QString section = QString(prefix + "Col:%1").arg(name);
        QString st = odf->value(section, "StoreType");
        inf._name = name;
        QString range = odf->value(section,"Range");
        QStringList parts = range.split(":");
        inf._isRaw = ( parts.size() == 4 || parts.size() == 3) && st != "Real";
        if ( st == "Long" ){
            inf._offset = _recordSize;
            _recordSize+=4;
            inf._type = itINT32;
        } if ( st == "Int" ){
            inf._offset = _recordSize;
            _recordSize+=4;
            inf._type = itINT32;
        } else if ( st == "String" ) {
            inf._offset = _recordSize;
            inf._type = itSTRING;
            _recordSize+=4;
        } else if ( st == "CoordBuf" ) {
            inf._offset = _recordSize;
            _recordSize+=4;
            inf._type = itBINARY;
        }
        else if ( st == "Real"){
            inf._offset = _recordSize;
            _recordSize+=8;
            inf._type  = itDOUBLE;

        } else if ( st == "Coord" ) {
            inf._offset = _recordSize;
            _recordSize += 16;
            inf._type = itCOORD2D;
        } else if ( st == "Coord3D" ) {
            inf._offset = _recordSize;
            _recordSize += 24;
            inf._type = itCOORD3D;
        }
        _columnInfo[col] = inf;

    }
}

void BinaryIlwis3Table::readData(char *memblock) {
    long posFile = 128;
    for(quint32 r = 0; r < _rows; ++r) {
        for(quint32 c = 0; c < _columns; ++c) {
            const ColumnInfo& info = _columnInfo.at(c);
            char *p = _records + r * _recordSize + info._offset;
            if( info._isRaw  || info._type == itINT32){
                *(long *)p = *(long *)(memblock + posFile);
                posFile += 4;
            } else if ( info._type == itDOUBLE) {
                *(double *)p = *(double *)(memblock + posFile);
                //v33 = *(double *)p;
                posFile += 8;
            } else if ( info._type == itCOORD2D) {
                memcpy(p,memblock + posFile,16);
                //c33= *(double *)p;
                posFile += 16;
            } else if ( info._type == itCOORD3D) {
                memcpy(p,memblock + posFile,24);
                posFile += 24;
            } else if ( info._type == itSTRING) {
                *(long *)p = *(long *)readString(memblock + posFile);
                posFile+= ((QString *)p)->size() + 1;
            } else if ( info._type == itBINARY) {
                long cnt = 0;
                *(long *)p = (long)readCoordList(memblock + posFile, cnt);
                posFile += cnt;
            }
        }
    }
}

QString *BinaryIlwis3Table::readString(char *mem) {
    char c;
    QString *txt = new QString();
    while((c = *(mem) ) != 0) {
        (*txt)+= c;
        ++mem;
    }
    return txt;
}

char *BinaryIlwis3Table::readCoordList(char *mem, long& count) {
    count = *(long *)mem;
    long noCoords = count / 16;
    vector<Coordinate> *coords = new vector<Coordinate>(noCoords);
    for(int i=0; i < noCoords; ++i) {
        double x = *(double *)(mem + 4 + i * sizeof(double) * 2);
        double y = *(double *)(mem + 4 + sizeof(double) * (i * 2 + 1));
        coords->at(i) = Coordinate(x,y,0);
    }
    count +=4;
    return (char *)coords;
}

bool BinaryIlwis3Table::get(quint32 row, quint32 column, double& v ) const {
    if(!check(row, column))
        return false;
    const ColumnInfo& info = _columnInfo.at(column);
    v = rUNDEF;
    char *p = moveTo(row,  info);
    if( info._isRaw  || info._type == itINT32){
        double *ddd = (double *)p;
        long raw = p != 0 ? *(long *) p : iUNDEF;
        v = raw;
    }
    else  if ( info._type == itDOUBLE) {
        v = *(double *)p;
    }
    return true;
}

bool BinaryIlwis3Table::get(quint32 row, quint32 column, Coordinate &c) const {
    if(!check(row, column))
        return false;
    const ColumnInfo& field = _columnInfo.at(column);
    char *p = moveTo(row,  field);
    c.x(*(double *)p);
    c.y(*(double *)(p + sizeof(double)));
    c.z(field._type == itCOORD3D ? *(double *)(p + sizeof(double)*2) : rUNDEF);

    return true;
}

bool BinaryIlwis3Table::get(quint32 row, quint32 column, QString& s) const {
    if(!check(row, column))
        return false;
    const  ColumnInfo& field = _columnInfo.at(column);
    char *p = moveTo(row,  field);
    s = *(QString *)p;


    return true;
}

bool BinaryIlwis3Table::get(quint32 row, quint32 column, vector<Coordinate> &coords) const {
    if(!check(row, column))
        return false;
    const ColumnInfo& field = _columnInfo.at(column);
    char *p = moveTo(row,  field);
    vector<Coordinate> *crds = ( vector<Coordinate> *)(*(long *)p);
    coords = *crds;

    return true;

}

bool BinaryIlwis3Table::get(quint32 row, quint32 column, vector<Coordinate2d> &coords) const {
    if(!check(row, column))
        return false;
    const ColumnInfo& field = _columnInfo.at(column);
    char *p = moveTo(row,  field);
    vector<Coordinate2d> *crds = ( vector<Coordinate2d> *)(*(long *)p);
    coords = *crds;

    return true;

}


inline bool BinaryIlwis3Table::check(quint32 row, quint32 col) const {
    if ( row >= _rows || col >= _columns) {
        kernel()->issues()->log(TR("Bounds error when accessing table"));
        return false;
    }
    return true;
}

quint32 BinaryIlwis3Table::index(const QString &colname) const
{
    for(quint32 i=0; i < _columns; ++i) {
        if ( _columnInfo[i]._name == colname)
            return i;
    }
    return iUNDEF;
}

quint32 BinaryIlwis3Table::rows() const
{
    return _rows;
}

quint32 BinaryIlwis3Table::columns() const
{
    return _columns;
}

QString BinaryIlwis3Table::columnName(int index)
{
    if ( index < _columnInfo.size()) {
        return _columnInfo[index]._name;
    }
    return sUNDEF;
}

inline char *BinaryIlwis3Table::moveTo(int row, const  ColumnInfo& fld) const{
    return (char *)(_records + row * _recordSize + fld._offset);
}

bool BinaryIlwis3Table::openOutput(const QString& basename, std::ofstream& output_file) {
    QFileInfo inf(basename);
    QString dir = context()->workingCatalog()->location().toLocalFile();
    QString filename = dir + "/" + inf.baseName() + ".tb#";
    output_file.open(filename.toLatin1(),ios_base::out | ios_base::binary | ios_base::trunc);
    if ( !output_file.is_open())
        return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,filename);
    char header[128];
    memset(header, 0, 128);
    output_file.write(header,128);

    return true;

}

void BinaryIlwis3Table::addStoreDefinition(const DataDefinition& def) {
    IDomain dmColumn = def.domain();
    IlwisTypes colType = dmColumn->ilwisType();
    ColumnInfo inf;
    if ( hasType(colType,itNUMERICDOMAIN) ) {
       auto nrange = def.range().dynamicCast<NumericRange>();
       RawConverter conv(nrange->min(), nrange->max(), nrange->step());
       inf._conv = conv;
       inf._type = colType;
    }
    else if ( hasType(colType, itITEMDOMAIN)) {
        RawConverter conv(0,1,0,2147483648,itINT32);
        inf._conv = conv;
        inf._type = itITEMDOMAIN;
    } else if ( hasType(colType, itTEXTDOMAIN) ||
                hasType(colType, itCOORDDOMAIN))  {
       inf._type = colType;
    }
    _columnInfo.push_back(inf);
}

void BinaryIlwis3Table::storeRecord(std::ofstream& output_file, const std::vector<QVariant>& rec, int skip) {
    for(int x=0; x < rec.size(); ++x) {
        if ( x == skip)
            continue;
        const RawConverter& conv = _columnInfo[x]._conv;
        IlwisTypes tp = _columnInfo[x]._type;
        if ( conv.isValid()) {
            if ( conv.isNeutral()) {
                if ( conv.storeType() == itINT32 && tp == itITEMDOMAIN) {
                    long val = rec[x].value<long>() + 1;
                    output_file.write((char *)&val, 4);
                }
                else if ( conv.storeType() != itDOUBLE)    {
                    long val = rec[x].value<long>();
                    output_file.write((char *)&val, 4);
                } else {
                    double val = rec[x].value<double>();
                    output_file.write((char *)&val, 8);
                }
            } else {
                double val = rec[x].value<double>();
                long raw = conv.real2raw(val);
                output_file.write((char *)&raw,  4);
            }
        }else {

            if ( tp == itTEXTDOMAIN) {
                QString s = rec[x].value<QString>();
                QByteArray bytes = s.toLocal8Bit();
                const char * ptr = bytes.data();
                output_file.write(ptr, s.size());
                char c = 0;
                output_file.write(&c, 1);

            } else if ( tp == itCOORDDOMAIN) {
                Coordinate crd =  rec[x].value<Coordinate>();
                double v = crd.x();
                output_file.write((char *)&v, 8);
                v = crd.y();
                output_file.write((char *)&v, 8);
                v = crd.z();
                output_file.write((char *)&v, 8);
            }
        }
    }
}



