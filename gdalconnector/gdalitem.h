#ifndef GDALITEM_H
#define GDALITEM_H

namespace Ilwis {
namespace Gdal {
class GDALItems : public QSet<Resource>
{
public:
    GDALItems(const QUrl& path, const UPContainerConnector &containerc, IlwisTypes extTypes);

private:
    void addItem(const QUrl &url, quint64 csyid, quint64 grfId, IlwisTypes tp =itRASTER, IlwisTypes extTypes=itUNKNOWN);
    quint64 addCsy(GdalHandle* handle, const QFileInfo &path, const QUrl &url);
};
}
}

#endif // GDALITEM_H
