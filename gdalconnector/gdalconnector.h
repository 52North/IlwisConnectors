#ifndef GDALCONNECTOR_H
#define GDALCONNECTOR_H

#include "gdalconnector_global.h"

namespace Ilwis{
namespace Gdal {

class GDALCONNECTORSHARED_EXPORT GdalConnector : public IlwisObjectConnector
{
public:
    GdalConnector(const Ilwis::Resource &resource, bool load=true);
    static IlwisTypes ilwisType(const QString &name);

    bool loadMetaData(IlwisObject* data);
    bool store(IlwisObject *, int);
    bool canUse(const Ilwis::Resource &resource);

    QString provider() const;
    void format(const QString&);
    QString format() const;
protected:
    static GDALDataType ilwisType2GdalType(IlwisTypes tp) ;
    QString constructOutputName(GDALDriverH hdriver) const;
    QString _filename; //TODO: might be better QFileInfo for Linux/Win filename case sensitivity
    QString _internalPath;
    QString _gdalShortName;
    GdalHandle* _handle;
};
}
}

#endif // GDALCONNECTOR_H
