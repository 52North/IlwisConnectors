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

bool WorkflowJSONConnector::openTarget() {

    if ( ioOptions().contains("inmemory") && ioOptions()["inmemory"].toBool() == true){
        _data.clear();
        _data.resize(100000);
        QBuffer *buf=  new QBuffer(&_data);
        buf->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        _datasource.reset( buf );
        return true;
    }else {
        QString filename = _resource.url(true).toLocalFile();
        QFileInfo inf(filename);
        if ( inf.suffix() != "ilwis"){
            filename = inf.absolutePath() + "/" + inf.fileName();
            QString correctUrl = QUrl::fromLocalFile(filename).toString();
            sourceRef().setUrl(correctUrl);
            sourceRef().setUrl(correctUrl,true);
        }
        QFile *file = new QFile(filename);

        if (file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
            _datasource.reset(file);
            return true;
        }
    }
    return false;
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
//    QJsonDocument document;
//    QJsonObject obj = createJSONWorkflowMetaData(workflow->resource());
//    document.setObject(obj);

//    QList<OVertex> nodes = workflow->getNodesWithExternalInput();
//    for(OVertex vid: nodes) {
//        NodeProperties props = workflow->nodeProperties(vid);
//        Resource metadata = mastercatalog()->id2Resource(props._operationid);
//        if ( metadata.isValid()){
//            obj = createJSONOperationMetadata(metadata);
//            document.object().insert("operations", obj);
//            boost::graph_traits<WorkflowGraph>::out_edge_iterator ei, ei_end;
//            for (boost::tie(ei,ei_end) = workflow->getOutEdges(vid); ei != ei_end; ++ei) {
//            }
//        }

//    }

    return true;
}

QString WorkflowJSONConnector::type() const
{
    return "workflow";
}

bool WorkflowJSONConnector::isReadOnly() const
{
    return false;
}

QString WorkflowJSONConnector::provider() const
{
    return "Json";
}

QVariant WorkflowJSONConnector::getProperty(const QString &) const
{
    return QString("");
}

Ilwis::IlwisObject *WorkflowJSONConnector::create() const
{
    return new Workflow(_resource);
}

ConnectorInterface *WorkflowJSONConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new WorkflowJSONConnector(resource, load, options);
}

QJsonObject WorkflowJSONConnector::createJSONWorkflow(const Resource &res)
{
}

QJsonObject WorkflowJSONConnector::createJSONWorkflowMetaData(const Resource& res){
    QJsonObject obj;
    obj.insert("longname", res.name());
    obj.insert("description", res.description());
    obj.insert("syntax", res["syntax"].toString());
    obj.insert("resource", QString("Ilwis"));
    obj.insert("keywords", res["keywords"].toString());
    obj.insert("inputparametercount", res["inparameters"].toString());
    obj.insert("outputparametercount", res["outparameters"].toString());

    return obj;
}

QJsonObject WorkflowJSONConnector::createJSONOperationMetadata(const Resource &res) {
    QJsonObject obj;
    obj.insert("longname", res.name());
    obj.insert("description",res.description());
    obj.insert("syntax", res["syntax"].toString());
    obj.insert("resource", QString("Ilwis"));
    obj.insert("keywords", res["keywords"].toString());
    obj.insert("inputparametercount", res["inparameters"].toString());
    obj.insert("outputparametercount", res["outparameters"].toString());
    obj.insert("final", res["final"].toString());

    return obj;
}

QJsonObject WorkflowJSONConnector::createJSONOperationList(const Resource& res) {
    QJsonObject obj;

    return obj;
}

QJsonObject WorkflowJSONConnector::createJSONOperationInputList(const Resource &res) {
    QJsonObject obj;

    return obj;
}

QJsonObject WorkflowJSONConnector::createJSONOperationOutputList(const Resource &res) {
    QJsonObject obj;

    return obj;
}

QJsonObject WorkflowJSONConnector::createJSONOperationConnectionList(const Resource &res) {
    QJsonObject obj;

    return obj;
}
