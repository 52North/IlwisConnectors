#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QVector>
#include "identity.h"
#include "kernel.h"
#include "resource.h"
#include "connectorinterface.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "gdalmodule.h"
#include "gdalproxy.h"
#include "gdalcatalogconnector.h"
#include "gdalitem.h"
#include "dataformat.h"
#include "mastercatalog.h"

using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *GdalCatalogConnector::create(const Resource &resource,bool) {
    return new GdalCatalogConnector(resource);

}

GdalCatalogConnector::GdalCatalogConnector(const Ilwis::Resource &resource) : CatalogConnector(resource), _type(resource.ilwisType())
{
}

inline uint qHash(const QFileInfo& inf ){
    return ::qHash(inf.canonicalFilePath());
}

bool GdalCatalogConnector::loadItems()
{
    QStringList filters = gdal()->getRasterExtensions();
    QVariantList ogrExt = DataFormat::getFormatProperties(DataFormat::fpEXTENSION, itFEATURE,"gdal");
    for(QVariant& ext : ogrExt)
        filters += "*." + ext.toString();
    filters.removeOne("*.hdr");

    std::vector<QUrl> files = containerConnector()->sources(filters
                                                      ,ContainerConnector::foFULLPATHS | ContainerConnector::foEXTENSIONFILTER);

    QSet<Resource> gdalitems;
    QList<Resource> folders;
    if (!gdal()->isValid()) {
        return ERROR1(ERR_NO_INITIALIZED_1,"gdal library");
    }
    for(const QUrl& url : files) {
        if ( mastercatalog()->url2id(url, itRASTER) == i64UNDEF && mastercatalog()->url2id(url, itFEATURE) == i64UNDEF) {
            QFileInfo file = containerConnector()->toLocalFile(url);
            if ( !file.isDir() ) {
                GDALItems items(url, containerConnector());
                gdalitems += items;
            } else {
                Resource resource(url, itCATALOG);
//                QUrl container;
                //drives have the format file:///c:/ while folders have file:///c:/myfolder; note the slash
                //drives must be recognized as container and formatted as such
//                QString name = url.toString();
//                int index = name.lastIndexOf(QRegExp("\\\\|/"));
//                if ( index != -1 && name[index - 1] == ':') { // is it a drive? (windows)
//                    if ( index == name.size() - 1)
//                        container = QUrl("file://");
//                    else
//                        container = QUrl(name.left(index));
//                }
//                resource.addContainer(container);
                //QFileInfo inf(resource.toLocalFile());
                QString filename = file.isRoot() ? file.absoluteFilePath() : file.fileName();
                resource.setName(filename);

                folders.push_back(resource);
            }
        }

    }
    mastercatalog()->addItems(gdalitems.toList());
    mastercatalog()->addItems(folders);

    return true;
}

bool GdalCatalogConnector::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    return true;
}

QString GdalCatalogConnector::provider() const
{
    return "gdal";
}
