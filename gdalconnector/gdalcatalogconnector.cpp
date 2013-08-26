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
#include "mastercatalog.h"

using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *GdalCatalogConnector::create(const Resource &item,bool) {
    return new GdalCatalogConnector(item);

}

GdalCatalogConnector::GdalCatalogConnector(const Ilwis::Resource &item) : CatalogConnector(item), _type(item.ilwisType())
{
}

inline uint qHash(const QFileInfo& inf ){
    return ::qHash(inf.canonicalFilePath());
}

bool GdalCatalogConnector::loadItems()
{
    QFileInfoList fileList;

    QUrl location = _location.url();
    if ( location.toString() == "file://") { // root will only contain drives (folders)
        fileList = QDir::drives();
        QFileInfoList dirs;
        foreach(QFileInfo inf , fileList) {
             QDir dir(inf.canonicalPath());
             dirs.append(dir.entryInfoList(QDir::Dirs));
        }
        fileList.append(dirs);;
    } else {
        QDir folder(location.toLocalFile());
        folder.setFilter(QDir::Dirs);
        if (!folder.exists()) {
            return ERROR1(ERR_COULD_NOT_OPEN_READING_1,folder.absolutePath());
        }
        fileList = folder.entryInfoList();
        QStringList namefilter = gdal()->rasterNameFilter();

        folder.setFilter(QDir::Files);
        QFileInfoList files = folder.entryInfoList(namefilter);
        fileList.append(files);

    }

    // remove duplicates, shoudnt happen but better save than sorry
    QSet<QFileInfo> reduced = fileList.toSet();
    fileList.clear();
    fileList = QList<QFileInfo>::fromSet(reduced);

    QSet<Resource> gdalitems;
    QList<Resource> folders;
    if (!gdal()->isValid()) {
        return ERROR1(ERR_NO_INITIALIZED_1,"gdal library");
    }
    foreach(QFileInfo file, fileList) {
        QUrl container = location;
        QString path = file.canonicalFilePath();
        QString loc = location.toLocalFile();
        if ( path.compare(loc,Qt::CaseInsensitive) == 0)
            container = file.canonicalPath();
        IlwisTypes tp = itGRID;
        QUrl res("file:///" + path);
        if ( mastercatalog()->resource2id(res, tp) == i64UNDEF) {
            if ( !file.isDir() ) {
                GDALItems items(path);
                gdalitems += items;
                //names[file.fileName()] = item.id();
            } else {
                Resource item(res, itCATALOG);
                //drives have the format file:///c:/ while folders have file:///c:/myfolder; note the slash
                //drives must be recognized as container and formatted as such
                int index = path.lastIndexOf(QRegExp("\\\\|/"));
                if ( index != -1 && path[index - 1] == ':') { // is it a drive? (windows)
                    if ( index == path.size() - 1)
                        container = QUrl("file://");
                    else
                        container = QUrl("file:///" + path.left(3));
                }
                item.setContainer(container);
                //QFileInfo inf(res.toLocalFile());
                QString name = file.isRoot() ? file.absoluteFilePath() : file.fileName();
                item.setName(name);

                folders.push_back(item);
            }
        }

    }
    mastercatalog()->addItems(gdalitems.toList());
    mastercatalog()->addItems(folders);

    return true;
}

bool GdalCatalogConnector::canUse(const QUrl &res) const
{
    if ( res.scheme() != "file")
        return false;

    if ( res.toString() == "file://") // root of file system
        return true;
    QFileInfo inf(res.toLocalFile());
    if ( !inf.isDir())
        return false;

    if ( !inf.exists())
        return false;

    return true;
}

QString GdalCatalogConnector::provider() const
{
    return "gdal";
}
