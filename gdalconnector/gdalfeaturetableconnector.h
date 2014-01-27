#ifndef GDALFEATURETABLECONNECTOR_H
#define GDALFEATURETABLECONNECTOR_H

namespace Ilwis{
    namespace Gdal {

        class GdalFeatureTableConnector : public GdalConnector{
            public:
                GdalFeatureTableConnector(const Ilwis::Resource &resource, bool load);

                IlwisObject *create() const;
                static ConnectorInterface *create(const Ilwis::Resource &resource, bool load);

                bool loadMetaData(IlwisObject* data);
                bool storeMetaData(Ilwis::IlwisObject *obj);

                bool loadBinaryData(IlwisObject *data);
                bool storeBinaryData(IlwisObject* obj);
        private:

        };

    }
}
#endif // GDALFEATURETABLECONNECTOR_H
