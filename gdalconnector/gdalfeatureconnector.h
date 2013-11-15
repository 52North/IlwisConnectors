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
                std::vector<SPFeatureI> fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
                std::vector<SPFeatureI> fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
                std::vector<SPFeatureI> fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
                std::vector<SPFeatureI> fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry, OGRwkbGeometryType type) const;
        };
    }
}
#endif // GDALFEATURECONNECTOR_H


