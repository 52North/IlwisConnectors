#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QHash>

#include "kernel.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "connectorfactory.h"
#include "spreadsheetconnectorsobjectfactory.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

SpreadSheetConnectorsObjectFactory::SpreadSheetConnectorsObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","SpreadSheetConnectors","")
{
}


bool SpreadSheetConnectorsObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() == "ilwis") // can't use anything marked as internal
        return false;

    if ( !hasType(resource.ilwisType(), itTABLE))
        return false;

    QString filename = resource.url().toLocalFile();
    if ( filename == "")
        return false;

    auto testFunc = [&](const QFileInfo& inf) -> bool {
        QString ext = inf.suffix().toLower();
        return  ext == "xls" || ext == "sdo";
    };

    QFileInfo info(resource.container().toLocalFile()); // possible case that the container is a gdal catalog
    if (info.isFile() && testFunc(info)) // for the moment a gdal catalog has to be another file
        return true;
    else {
        QFileInfo info (resource.toLocalFile());
        return testFunc(info);
    }
    return false;
}

IlwisObject *SpreadSheetConnectorsObjectFactory::create(const Resource &resource, const PrepareOptions &options) const
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    IlwisObjectConnector *connector = factory->createFromResource<IlwisObjectConnector>(resource, "spreadsheet", options);

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
   IlwisObject *object = createObject(connector, options);
   if ( object)
       return object;

   delete connector;
   return 0;
}

IlwisObject *SpreadSheetConnectorsObjectFactory::createTable(const Resource& resource){

}


