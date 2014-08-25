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

    bool loadMetadata(FeatureCoverage *fcoverage) const;
    bool loadData(FeatureCoverage *fcoverage) const;

private:
    Resource _resource;

    void setFeatureCount(FeatureCoverage *fcoverage) const;
    void setSpatialMetadata(FeatureCoverage *fcoverage) const;
    QSqlQuery selectGeometries(const QList<MetaGeometryColumn> metaGeometry) const;
    geos::geom::Geometry* createGeometry(QSqlQuery &query, MetaGeometryColumn &meta) const;
};


}
}

#endif // POSTGRESQLFEATURECOVERAGELOADER_H
