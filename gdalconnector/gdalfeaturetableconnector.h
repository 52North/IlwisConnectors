#ifndef GDALFEATURETABLECONNECTOR_H
#define GDALFEATURETABLECONNECTOR_H

namespace Ilwis{
    namespace Gdal {

        class GdalFeatureTableConnector : public GdalConnector{
            public:
                GdalFeatureTableConnector(const Ilwis::Resource &resource, bool load,const PrepareOptions& options=PrepareOptions());

                IlwisObject *create() const;
                static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const PrepareOptions& options=PrepareOptions());

                bool loadMetaData(IlwisObject* data,const PrepareOptions&);
                bool storeMetaData(Ilwis::IlwisObject *obj);

                bool loadData(IlwisObject *data);
                bool storeBinaryData(IlwisObject* obj);
        private:

        };

    }
}
#endif // GDALFEATURETABLECONNECTOR_H
