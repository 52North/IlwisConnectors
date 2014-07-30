#include "kernel.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "spreadsheetconnectorsobjectfactory.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
#include "spreadsheetconnectorsmodule.h"
#include "exceltableconnector.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

SpreadSheetConnectorsModule::SpreadSheetConnectorsModule(QObject *parent) : Module(parent, "SpreadSheetConnectorsModule", "iv40","1.0")
{

}

SpreadSheetConnectorsModule::~SpreadSheetConnectorsModule()
{

}

QString SpreadSheetConnectorsModule::getInterfaceVersion() const
{
      return "iv40";
}

QString SpreadSheetConnectorsModule::getName() const
{
    return "Spread sheet plugin";
}

QString SpreadSheetConnectorsModule::getVersion() const
{
    return "1.0";
}

void SpreadSheetConnectorsModule::prepare()
{
    SpreadSheetConnectorsObjectFactory *factory = new SpreadSheetConnectorsObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;
    cfactory->addCreator(itTABLE ,"spreadsheet", ExcelTableConnector::create);

}
