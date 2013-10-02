#ifndef GDALFEATURECONNECTOR_H
#define GDALFEATURECONNECTOR_H

namespace Ilwis{
    namespace Gdal {
        class GdalFeatureConnector : public GdalConnector
        {
        public:
            GdalFeatureConnector(const Ilwis::Resource &resource, bool load=true);
            static IlwisTypes ilwisType(const QString &name);

            bool loadMetaData(IlwisObject* data);
            bool store(IlwisObject*, int);
//            bool canUse(const Ilwis::Resource &resource);

            static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
            Ilwis::IlwisObject *create() const;

//            QString provider() const;
//            void format(const QString&);
//            QString format() const;
        protected:
            bool reportError(GDALDatasetH dataset) const;
            static GDALDataType ilwisType2GdalType(IlwisTypes tp) ;
            QString constructOutputName(GDALDriverH hdriver) const;
            QString _filename;
            QString _internalPath;
            GDALDatasetH _dataSet;
            QString _gdalShortName;
        };
    }
}
#endif // GDALFEATURECONNECTOR_H


