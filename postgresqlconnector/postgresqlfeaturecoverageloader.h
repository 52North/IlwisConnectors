#ifndef POSTGRESQLFEATURECOVERAGELOADER_H
#define POSTGRESQLFEATURECOVERAGELOADER_H

namespace Ilwis {

namespace Postgresql {

struct MetaGeometryColumn;

class PostgresqlFeatureCoverageLoader
{
public:
    PostgresqlFeatureCoverageLoader(const Resource resource);
    ~PostgresqlFeatureCoverageLoader();

    bool loadMetadata(FeatureCoverage *fcoverage, const IOOptions &options) const;
    bool loadData(FeatureCoverage *fcoverage, const IOOptions &options) const;
    bool storeData(FeatureCoverage *fcoverage, const IOOptions &options) const;

private:
    Resource _resource;

    void setFeatureCount(FeatureCoverage *fcoverage) const;
    void setSpatialMetadata(FeatureCoverage *fcoverage) const;
    void setSubfeatureSemantics(Ilwis::FeatureCoverage *fcoverage, Ilwis::IDomain &semantics) const;

    QString selectGeometries(const QList<MetaGeometryColumn> &metaGeometry) const;
    geos::geom::Geometry* createGeometry(QSqlQuery &query, MetaGeometryColumn &meta) const;
};


}
}

#endif // POSTGRESQLFEATURECOVERAGELOADER_H
