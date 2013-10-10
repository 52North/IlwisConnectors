#ifndef GDALFEATURECONNECTOR_H
#define GDALFEATURECONNECTOR_H

namespace Ilwis{
    namespace Gdal {
        class GdalFeatureConnector : public CoverageConnector{
            public:
                GdalFeatureConnector(const Ilwis::Resource &resource, bool load=true);

                bool loadMetaData(IlwisObject* data);
                bool loadBinaryData(IlwisObject* data);
                bool store(IlwisObject *obj, IlwisTypes type);

                static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
                Ilwis::IlwisObject *create() const;
            protected:
                IlwisTypes translateOGRType(OGRwkbGeometryType type) const;
                bool fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry, quint64& rec) const;
                bool fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry, quint64& rec) const;
                bool fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry, quint64& rec) const;
                bool fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry, quint64& rec) const;
        };
    }
}
#endif // GDALFEATURECONNECTOR_H


