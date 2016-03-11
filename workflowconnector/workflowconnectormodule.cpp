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
#include "workflowconnectorobjectfactory.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
#include "workflowconnectormodule.h"


using namespace Ilwis;
using namespace WorkflowConnector;

WorkflowConnectorModule::WorkflowConnectorModule(QObject *parent) : Module(parent, "WorkflowConnectorModule", "iv40","1.0")
{

}

QString WorkflowConnectorModule::getInterfaceVersion() const
{
      return "iv40";
}

QString WorkflowConnectorModule::getName() const
{
    return "Spread sheet plugin";
}

QString WorkflowConnectorModule::getVersion() const
{
    return "1.0";
}

void WorkflowConnectorModule::finalizePreparation()
{

}

void WorkflowConnectorModule::prepare()
{
    WorkflowConnectorObjectFactory *factory = new WorkflowConnectorObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;
  //  cfactory->addCreator(itWORKFLOW,"workflow",WorkflowJSONConnector::create);


  //  cfactory->addCreator("workflow","Workflows", WorkflowJSONConnector::create);

    kernel()->issues()->log("Loaded Workflow module",IssueObject::itMessage);

}
