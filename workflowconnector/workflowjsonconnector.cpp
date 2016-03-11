#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "workflow/workflow.h"
#include "workflow/workflowoperationimplementation.h"
#include "ilwisobjectconnector.h"
#include "workflowjsonconnector.h"

using namespace Ilwis;
using namespace WorkflowConnector;


WorkflowJSONConnector::WorkflowJSONConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{

}

bool WorkflowJSONConnector::loadMetaData(IlwisObject *object, const IOOptions &)
{



    return false;
}

bool WorkflowJSONConnector::loadData(IlwisObject *, const IOOptions &options)
{
    return false;
}

bool WorkflowJSONConnector::store(IlwisObject *object, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(object);
    QJsonDocument document;
    QJsonObject obj = createJSONOperationMetaData(workflow->resource());
    document.setObject(obj);

    QList<OVertex> nodes = workflow->getNodesWithExternalInput();
    for(OVertex vid: nodes) {
        NodeProperties props = workflow->nodeProperties(vid);
        Resource metadata = mastercatalog()->id2Resource(props._operationid);
        if ( metadata.isValid()){
            obj = createJSONOperationMetaData(metadata);
            document.object().insert("operations", obj);
            boost::graph_traits<WorkflowGraph>::out_edge_iterator ei, ei_end;
            for (boost::tie(ei,ei_end) = workflow->getOutEdges(vid); ei != ei_end; ++ei) {
            }
        }

    }

    return false;
}

QString WorkflowJSONConnector::format() const
{
    return "jsonworkflow";
}

bool WorkflowJSONConnector::isReadOnly() const
{
    return false;
}

QJsonObject WorkflowJSONConnector::createJSONOperationMetaData(const Resource& res){
    QJsonObject obj;
    obj.insert("longname", res.name());
    obj.insert("description",res.description());
    obj.insert("syntax", res["syntax"].toString());
    obj.insert("resource",QString("Ilwis"));
    obj.insert("keywordss", res["keywords"].toString());
    obj.insert("inputparametercount",res["inparameters"].toString());
    obj.insert("outputparametercount",res["outparameters"].toString());

    return obj;
}
