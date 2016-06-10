#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
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
    QJsonDocument document;
    QJsonObject allworkflows;
    QJsonArray workflows;   // top-level object, contains array of workflows

    QJsonObject single = createJSONWorkflow(workflow->resource());
    single["metadata"] = createJSONWorkflowMetaData(workflow->resource());

    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = workflow->getNodeIterators();
    int size = nodeIterators.second - nodeIterators.first;

    QJsonArray operations;
    for (auto iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {
        NodeProperties nodeData = workflow->nodeProperties(*iter);
        Resource res = mastercatalog()->id2Resource(nodeData._operationid);
        IOperationMetaData metadata;
        metadata.prepare(res);
        if ( res.isValid()){
            QJsonObject operation;
            QJsonObject obj = createJSONOperationMetadata(res);
            operation["id"] = QString::number(nodeData._id);
            operation["metadata"] = obj;
            operation["inputs"] = createJSONOperationInputList(metadata);
            operation["outputs"] = createJSONOperationOutputList(metadata);
            operations.append(operation);
        }
    }
    single["operations"] = operations;

    QJsonArray connect;
    single["connections"] = connect;

    workflows.append(single);

    allworkflows["workflows"] = workflows;
    document.setObject(allworkflows);

    // open output file, and write Json stream to it
    if (!openTarget())
        return false;

    QTextStream stream(_datasource.get());
    stream << document.toJson();
    stream.device()->close();

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
    QJsonObject workflow;

    workflow["id"] = QString("0");
    return workflow;
}

QJsonObject WorkflowJSONConnector::createJSONWorkflowMetaData(const Resource& res){
    QJsonObject meta;
    meta.insert("longname", res.name());
    meta.insert("description", res.description());
    meta.insert("syntax", res["syntax"].toString());
    meta.insert("resource", QString("Ilwis"));
    meta.insert("keywords", res["keywords"].toString());
    meta.insert("inputparametercount", res["inparameters"].toString());
    meta.insert("outputparametercount", res["outparameters"].toString());

    return meta;
}

QJsonObject WorkflowJSONConnector::createJSONOperationMetadata(const Resource &res) {
    QJsonObject meta;
    meta["longname"] = res.name();
    meta["description"] = res.description();
    meta["syntax"] = res["syntax"].toString();
    meta["resource"] = QString("Ilwis");
    meta["keywords"] = res["keywords"].toString();
    meta["inputparametercount"] = res["inparameters"].toString();
    meta["outputparametercount"] = res["outparameters"].toString();
    meta["final"] = res["final"].toString();

    return meta;
}

QJsonObject WorkflowJSONConnector::createJSONOperationList(const Resource& res) {
    QJsonObject obj;

    return obj;
}

QJsonArray WorkflowJSONConnector::createJSONOperationInputList(const IOperationMetaData &meta) {
    QJsonArray inputs;

    std::vector<SPOperationParameter> params = meta->getInputParameters();
    foreach (SPOperationParameter parameter, params) {
        QJsonObject input;
        input["term"] = parameter->term();
        input["name"] = parameter->name();
        input["type"] = Ilwis::TypeHelper::type2HumanReadable(parameter->type());
        input["id"] = QString("%1").arg(parameter->id());
        input["optional"] = parameter->isOptional();
        input["description"] = parameter->description();

        inputs.append(input);
    }

    return inputs;
}

QJsonArray WorkflowJSONConnector::createJSONOperationOutputList(const IOperationMetaData &meta) {
    QJsonArray outputs;

    return outputs;
}

QJsonArray WorkflowJSONConnector::createJSONOperationConnectionList(const Resource &res) {
    QJsonArray connections;

    return connections;
}
