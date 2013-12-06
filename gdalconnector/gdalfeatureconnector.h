#ifndef GDALFEATURECONNECTOR_H
#define GDALFEATURECONNECTOR_H

namespace Ilwis{
namespace Gdal {

struct SourceHandles{
    SourceHandles(OGRDataSourceH source=0, const std::vector<OGRLayerH>& layer=std::vector<OGRLayerH>()) : _source(source), _layers(layer) {}
    OGRDataSourceH _source;
    std::vector<OGRLayerH> _layers;
};
typedef std::vector<SourceHandles> FeatureSetHandles;

class GdalFeatureConnector : public CoverageConnector{
public:
    GdalFeatureConnector(const Ilwis::Resource &resource, bool load=true);

    bool loadMetaData(IlwisObject* data);
    bool loadBinaryData(IlwisObject* data);
    bool store(IlwisObject *obj, int);

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
    Ilwis::IlwisObject *create() const;
protected:

private:
    OGRSFDriverH _driver;

    enum OutputState { osIndex2Layer=1,osType2Layer=2, osLayer2DataSource=4};
    struct FillerColumnDef{
        FillerColumnDef(QVariant(GdalFeatureConnector::* func)(OGRFeatureH,  int, SPRange), int i, SPRange r): fillFunc(func), index(i), range(r){}
        QVariant (GdalFeatureConnector::* fillFunc)(OGRFeatureH,  int, SPRange);
        int index;
        SPRange range;
    };
    QVariant fillEmptyColumn(OGRFeatureH, int, SPRange);
    QVariant fillStringColumn(OGRFeatureH featureH, int colIntex, SPRange range);
    QVariant fillIntegerColumn(OGRFeatureH featureH, int colIntex, SPRange range);
    QVariant fillDoubleColumn(OGRFeatureH featureH, int colIntex, SPRange range);
    QVariant fillDateTimeColumn(OGRFeatureH featureH, int colIntex, SPRange range);
    IlwisTypes translateOGRType(OGRwkbGeometryType type) const;
    void fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry, std::vector<Ilwis::Geometry> &outGeoms) const;
    void fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry, std::vector<Ilwis::Geometry> &outGeoms) const;
    void fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry, std::vector<Ilwis::Geometry> &outGeoms) const;
    void fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry, OGRwkbGeometryType type, std::vector<Ilwis::Geometry> &outGeoms) const;
    OGRDataSourceH createFileBasedDataSource(const QString &postfix, const QFileInfo &fileinfo) const;
    OGRLayerH createLayer(const QString &name, OGRwkbGeometryType type, OGRSpatialReferenceH srs, OGRDataSourceH source);
    bool oneLayerPerFeatureType(const IFeatureCoverage &features);
    bool createAttributes(const ITable &tbl, OGRLayerH layer, const std::vector<OGRFieldDefnH> &fielddefs,std::vector<bool>& validAttributes);
    bool loadDriver();
    OGRwkbGeometryType ilwisType2GdalFeatureType(IlwisTypes tp);
    void setAttributes(OGRFeatureH hfeature, Ilwis::SPFeatureI& feature, const std::vector<bool> &validAttributes, const std::vector<Ilwis::ColumnDefinition> &def);
    bool setDataSourceAndLayers(const IFeatureCoverage &features, std::vector<SourceHandles> &datasources,std::vector<bool>& validAttributes);
    OGRGeometryH createLine2D(const SPFeatureI &feature);
    OGRGeometryH createPoint2D(const SPFeatureI &feature);
    OGRGeometryH createPolygon2D(const SPFeatureI &feature);

    static int ilwisType2Index(IlwisTypes);
    bool createDataSourceAndLayers(IlwisTypes types,
                                   const QString &postfix,
                                   const IFeatureCoverage &features,
                                   OGRSpatialReferenceH srs,
                                   const std::vector<OGRFieldDefnH> &fielddefs,
                                   std::vector<SourceHandles> &datasources,
                                   std::vector<bool> &validAttributes);
};
}
}
#endif // GDALFEATURECONNECTOR_H


