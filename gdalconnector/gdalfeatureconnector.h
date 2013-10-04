#ifndef GDALFEATURECONNECTOR_H
#define GDALFEATURECONNECTOR_H

namespace Ilwis{
    namespace Gdal {
        class GdalFeatureConnector : public CoverageConnector{
            public:
                GdalFeatureConnector(const Ilwis::Resource &resource, bool load=true);

                bool loadMetaData(IlwisObject* data);
                bool loadBinaryData(IlwisObject* data) { return false; }
                bool store(IlwisObject *obj, IlwisTypes type);

                static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
                Ilwis::IlwisObject *create() const;
            protected:
                IlwisTypes getFeatureType(OGRLayerH hLayer) const;
                void reportError(OGRDataSourceH dataSource) const;


                OGRDataSourceH _dataSource;


        };
    }
}
#endif // GDALFEATURECONNECTOR_H


