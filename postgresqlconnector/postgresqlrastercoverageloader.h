#ifndef POSTGRESQLRASTERCOVERAGELOADER_H
#define POSTGRESQLRASTERCOVERAGELOADER_H

namespace Ilwis {

namespace Postgresql {

struct MetaRasterColumn;

class PostgresqlRasterCoverageLoader
{

public:
    PostgresqlRasterCoverageLoader(const Resource &resource, const IOOptions &options);
    ~PostgresqlRasterCoverageLoader();

    bool loadMetadata(RasterCoverage *fcoverage) const;
    bool loadData(RasterCoverage *coverage) const;
    bool storeData(RasterCoverage *fcoverage) const;
    static char hex2dec(char * str);
    std::string getString(std::string str) const;
    unsigned long convtodecnum(char *hex) const;
    double getNodataVal(QString & val, QString & pixel_type);

    QString getBandPixelType(const QList<MetaRasterColumn> &metaGeometry) const;
    QString getAdditionalInfo(const QList<MetaRasterColumn> &metaGeometry) const;
    IDomain setDomain(QString pixel_type) const;

    void setSpatialMetadata(RasterCoverage *coverage) const;
    QString selectRasters(const QList<MetaRasterColumn> &metaGeometry) const;
    IlwisTypes setStoreType(QString pixel_type, DataDefinition inf) const;
private:
    Resource _resource;
    IOOptions _options;




};
}
}
#endif // POSTGRESQLRASTERCOVERAGELOADER_H
