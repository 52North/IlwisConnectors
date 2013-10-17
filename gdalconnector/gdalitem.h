#ifndef GDALITEM_H
#define GDALITEM_H

namespace Ilwis {
namespace Gdal {
class GDALItems : public QSet<Resource>
{
public:
    GDALItems(const QString& path);

private:
    void addItem(const QUrl &url, quint64 csyid, quint64 grfId, IlwisTypes tp =itRASTER);
    quint64 addCsy(GdalHandle* handle, const QString& path);
};
}
}

#endif // GDALITEM_H
