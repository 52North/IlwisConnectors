#ifndef GDALCONNECTOR_H
#define GDALCONNECTOR_H

#include "gdalconnector_global.h"

namespace Ilwis{
namespace Gdal {

class GDALCONNECTORSHARED_EXPORT GdalConnector : public IlwisObjectConnector
{
public:
    GdalConnector(const Ilwis::Resource &item, bool load=true);
    static IlwisTypes ilwisType(const QString &name);

    bool loadMetaData(IlwisObject* data);
    bool store(IlwisObject *, int);
    bool canUse(const Ilwis::Resource &item);

    QString provider() const;
    void format(const QString&);
    QString format() const;
protected:
    static GDALDataType ilwisType2GdalType(IlwisTypes tp) ;
    QString _filename;
    QString _internalPath;
    GDALDatasetH _dataSet;
    QString _gdalShortName;
};
}
}

#endif // GDALCONNECTOR_H
