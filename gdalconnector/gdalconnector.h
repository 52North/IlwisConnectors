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
    bool canUse(const Ilwis::Resource &item);

protected:
    QString _filename;
    QString _internalPath;
    GDALDatasetH _dataSet;
};
}
}

#endif // GDALCONNECTOR_H
