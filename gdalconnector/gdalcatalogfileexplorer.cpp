#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QVector>
#include "identity.h"
#include "kernel.h"
#include "resource.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "dataformat.h"
#include "gdalmodule.h"
#include "gdalproxy.h"
#include "gdalitem.h"
#include "mastercatalog.h"
#include "gdalcatalogfileexplorer.h"

using namespace Ilwis;
using namespace Gdal;

REGISTER_CATALOGEXPLORER(GdalCatalogFileExplorer)

Ilwis::CatalogExplorer *GdalCatalogFileExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new GdalCatalogFileExplorer(resource, options);
}

GdalCatalogFileExplorer::GdalCatalogFileExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{
}
std::vector<Ilwis::Resource> GdalCatalogFileExplorer::loadItems(const IOOptions &)
{
    QFileInfo containerInf = source().toLocalFile();
    if ( containerInf.isDir()){
        ERROR2(ERR_COULD_NOT_CONVERT_2,"container", "file" );
        return std::vector<Ilwis::Resource>();
    }
    std::vector<Ilwis::Resource> result;
    QString query = QString("Select itemid from mastercatalog where container='%1'").arg(source().url().toString());
    InternalDatabaseConnection db;
    if (db.exec(query)) {
        if ( db.next()){
            do{
                quint64 id = db.value(0).toULongLong();
                Resource resource = mastercatalog()->id2Resource(id);
                if ( resource.isValid())
                    result.push_back(resource);

            }while ( db.next());
        }else {
            bool prev = kernel()->issues()->silent();
            kernel()->issues()->silent(true); // error messages during scan are not needed
            GDALItems gdalitems(containerInf);
            std::vector<Resource> items;
            for( const auto& res : gdalitems){
                Resource resource(res);
                resource.createTime(Time(containerInf.created()));
                resource.modifiedTime(Time(containerInf.lastModified()));
                items.push_back(resource);
            }
            mastercatalog()->addItems(items);
            kernel()->issues()->silent(prev);
        }
    }
    return result;
}

bool GdalCatalogFileExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    QFileInfo fileInfo(resource.url().toLocalFile());
    if ( fileInfo.isFile()) { // must be a file and extension must be supported by gdal
        QStringList lst = gdal()->getExtensions(itFEATURE | itRASTER);
        QString ext = "*." + fileInfo.suffix();
        bool ok = lst.indexOf(ext) != -1;
        return ok;
    }
    return false;
}

QString GdalCatalogFileExplorer::provider() const
{
    return "gdal";
}

QFileInfo GdalCatalogFileExplorer::toLocalFile(const QUrl &datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}


