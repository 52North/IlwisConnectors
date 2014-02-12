#ifndef FEATURECONNECTOR_H
#define FEATURECONNECTOR_H

namespace geos {
    namespace geom{
    class Polygon;
    }
}
namespace Ilwis {

class FeatureCoverage;

namespace Ilwis3{

class BinaryIlwis3Table;

class FeatureConnector : public CoverageConnector
{
public:
    FeatureConnector(const Ilwis::Resource &resource, bool load);
    bool loadMetaData(Ilwis::IlwisObject *data);
    bool loadBinaryData(Ilwis::IlwisObject *obj);
    bool storeMetaData(IlwisObject *obj) ;
    bool storeBinaryData(IlwisObject *obj);

    static ConnectorInterface *create(const Resource &resource, bool load);
    Ilwis::IlwisObject *create() const;
    void calcStatics(const IlwisObject *obj,NumericStatistics::PropertySets set) const;


private:
    bool loadBinaryPoints(FeatureCoverage *fcoverage);
    bool loadBinarySegments(FeatureCoverage *fcoverage);
    bool loadBinaryPolygons(FeatureCoverage *fcoverage);
    bool loadBinaryPolygons30(FeatureCoverage *fcoverage, ITable &tbl);
    bool loadBinaryPolygons37(FeatureCoverage *fcoverage, ITable& tbl);
    //bool readRing(QDataStream &stream,boost::geometry::model::ring<Coordinate2d>& ring);
    geos::geom::CoordinateArraySequence *readRing(QDataStream &stream);
    bool getRings(Ilwis::FeatureCoverage *fcoverage, qint32 startIndex, const BinaryIlwis3Table &topTable, const BinaryIlwis3Table& polTable, std::vector<geos::geom::LinearRing *> *rings);
    bool isForwardStartDirection(const BinaryIlwis3Table &topTable, qint32 colForward, qint32 colBackward, qint32 colCoords, long index);

    void writeCoords(std::ofstream &output_file, const std::vector<geos::geom::Coordinate>* coords, bool singleton=false);
    bool storeMetaPolygon(FeatureCoverage *fcov, const QString &dataFile);
    bool storeMetaLine(FeatureCoverage *fcov, const QString &dataFile);
    bool storeMetaPoint(FeatureCoverage *fcov, const QString &filepath);
    bool storeBinaryDataPolygon(Ilwis::FeatureCoverage *fcov, const QString &baseName);
    bool storeBinaryDataPoints(FeatureCoverage *fcov, const QString &baseName);
    bool storeBinaryDataLine(FeatureCoverage *fcov, const QString &baseName);
    bool storeMetaData(Ilwis::FeatureCoverage *fcov, IlwisTypes type);
    bool storeBinaryData(Ilwis::FeatureCoverage *fcov, bool isMulti, IlwisTypes type);

    void storeColumn(const QString &colName, const QString &domName, const QString &domInfo, const QString &storeType);

    void writeCoord(std::ofstream& output_file, const geos::geom::Coordinate& crd) {
        double x = crd.x;
        double y = crd.y;
        output_file.write((char *)&x, 8);
        output_file.write((char *)&y, 8);
    }
    void writeLine(const geos::geom::LineString *line, std::ofstream &output_file, long raw);
    void writePolygon(const geos::geom::Polygon *polygon, std::ofstream &output_file, double raw);
    void writePoint(const geos::geom::Point *point, std::ofstream &output_file, long raw);
    void addFeatures(map<quint32, vector<geos::geom::Geometry *> > &geometries, FeatureCoverage *fcoverage, ITable &tbl, IlwisTypes tp);
    bool storeBinaryDataTable(IlwisObject *obj, IlwisTypes tp, const QString &baseName);
};
}
}

#endif // FEATURECONNECTOR_H
