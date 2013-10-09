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
                GeometryType fillFeature(OGRGeometryH geometry, int& rec) const;
                GeometryType fillPoint(OGRGeometryH geometry, int& rec) const;
                GeometryType fillLine(OGRGeometryH geometry, int& rec) const;
                GeometryType fillPolygon(OGRGeometryH geometry, int& rec) const;
        };
    }
}
#endif // GDALFEATURECONNECTOR_H


