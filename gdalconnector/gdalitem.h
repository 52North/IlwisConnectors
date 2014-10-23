#ifndef GDALITEM_H
#define GDALITEM_H

namespace Ilwis {
namespace Gdal {
class GDALItems : public std::set<Resource>
{
public:
    GDALItems(const QUrl& path, const QFileInfo &localFile, IlwisTypes extTypes);

private:
    quint64 addItem(GdalHandle* handle, const QUrl &url, quint64 csyid, quint64 grfId, IlwisTypes tp =itRASTER, IlwisTypes extTypes=itUNKNOWN, quint64 sz=i64UNDEF);
    quint64 addCsy(GdalHandle* handle, const QString &path, const QUrl &url, bool message=true);
    QString dimensions(GdalHandle* handle) const;
    std::map<QString, QString> kvp2Map(char **kvplist);
    void handleComplexDataSet(const std::map<QString, QString> &datasetdesc);
    quint64 numbertype2domainid(const QString &numbertype) const;
    quint64 findSize(const QFileInfo &inf);
};
}
}

#endif // GDALITEM_H
