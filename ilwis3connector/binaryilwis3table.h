#ifndef BINARYILWIS3TABLE_H
#define BINARYILWIS3TABLE_H

namespace Ilwis {

namespace Ilwis3{


class BinaryIlwis3Table
{
public:
    BinaryIlwis3Table();
    ~BinaryIlwis3Table();

    bool load(const ODF &odf, const QString &prfix="");

    bool get(quint32 row, quint32 column, double &v) const;
    bool get(quint32 row, quint32 column, Coordinate &c) const;
    bool get(quint32 row, quint32 column, QString &s) const;
    bool get(quint32 row, quint32 column, vector<Coordinate>& coords) const;
    bool get(quint32 row, quint32 column, vector<Coordinate2d> &coords) const;
    quint32 index(const QString& colname) const;
    quint32 rows() const;
    quint32 columns() const;
    QString columnName(int index);
    void addStoreDefinition(const DataDefinition &def);
    void storeRecord(std::ofstream &output_file, const std::vector<QVariant> &rec, int skip=iUNDEF);
    bool openOutput(const QString &basename, std::ofstream &output_file);
private:
    struct ColumnInfo{
        bool _isRaw;
        quint32 _offset;
        IlwisTypes _type;
        QString _name;
        RawConverter _conv;
    };
    quint32 _rows;
    quint32 _columns;
    quint32 _recordSize;
    char * _records;
    QVector<ColumnInfo> _columnInfo;

    void getColumnInfo(const ODF &odf, const QString &prfix="");
    void readData(char *memblock);
    QString *readString(char *mem);
    char *readCoordList(char *mem, long &count);
    char *moveTo(int row, const ColumnInfo &fld) const;
    bool check(quint32 row, quint32 col) const;

};
}
}

#endif // BINARYILWIS3TABLE_H
