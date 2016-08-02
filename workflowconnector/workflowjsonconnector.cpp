#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "operationmetadata.h"
#include "operationExpression.h"
#include "operation.h"
#include "jsonconfig.h"
#include "workflow/workflow.h"
#include "WorkflowJSONConnector.h"

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

    QString workflowPath = workflow->resourceRef().container(true).toLocalFile();
    _config.loadSystemConfig(QString("%1/config.json").arg(workflowPath));
    _config.loadUserConfig(QString("%1/userconfig.json").arg(workflowPath));

    QJsonObject single = createJSONWorkflow(workflow->resource());
    single["metadata"] = createJSONWorkflowMetaData(workflow->resource());

    // deal with all the operations
    QJsonArray operations;

    QList<OVertex> inputOperations = workflow->getNodesWithExternalInput();
    parseInputNodeArguments(workflow, inputOperations);

    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = workflow->getNodeIterators();

    for (auto iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {
        OVertex v = *iter;
        NodeProperties nodeData = workflow->nodeProperties(*iter);
        Resource res = mastercatalog()->id2Resource(nodeData._operationid);
        IOperationMetaData metadata;
        metadata.prepare(res);
        if (res.isValid()) {
            QJsonObject operation;
            QJsonObject obj = createJSONOperationMetadata(res);
            operation["id"] = QString::number(v);
            operation["metadata"] = obj;
            operation["inputs"] = createJSONOperationInputList(workflow, v);
            operation["outputs"] = createJSONOperationOutputList(workflow, v);
            operations.append(operation);
        }
    }
    single["operations"] = operations;

    single["connections"] = createJSONOperationConnectionList(workflow);

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

void WorkflowJSONConnector::parseOutputNames(OperationExpression expression)
{
    QString expr = expression.input<QString>(0); // we dont want the script command, just its tail
    int assignIndex = expr.indexOf("=");
    QString rightTerm = expr.mid(assignIndex + 1);
    QString leftTerm = expr.left(assignIndex);
    QStringList outs = leftTerm.split("},");
    for(auto output : outs){
        QString res ;
        int index = output.lastIndexOf("/");
        if ( index != -1){
            res = output.mid(index + 2);
        }else
            res = output;
        if (!res.endsWith("}")) // due to the nature how split works the last '}' will have been dropped
            res += "}";
        _outputNames.append(res);
    }
    _expression = rightTerm;
}

void WorkflowJSONConnector::parseInputNodeArguments(Workflow *workflow, const QList<OVertex> &inputNodes)
{
    // cache inputs, might go into multiple operations
    QMap<SPAssignedInputData, QString> inputs;

    quint16 inputParamIndex = 0;
    for (OVertex inputOperation : inputNodes) {
        QStringList requireds;
        QStringList optionals;
        IOperationMetaData meta = workflow->getOperationMetadata(inputOperation);
        meta->parametersFromSyntax(requireds, optionals);
        QStringList arguments;
        for (int i = 0 ; i < meta->inputParameterCount() ; i++) {
            arguments.push_back(""); // initialize empty arguments
        }

        // ------------ parse required arguments
        for (int argIndex = 0 ; argIndex < requireds.size(); argIndex++) {
            if ( !workflow->hasInputAssignment(inputOperation, argIndex)) {
                continue; // implicit input
            } else {
                SPAssignedInputData inputData = workflow->getAssignedInputData({inputOperation, argIndex});
                if (inputs.contains(inputData)) {
                    // shared over multiple operations
                    arguments.replace(argIndex, inputs.value(inputData));
                } else {
                    QString argument;
                    if (inputData->value.size() > 0) {
                        argument = inputData->value;    // pre-filled input value
                    } else {
                        argument = QString("");
                        inputParamIndex++;
                    }
                    inputs.insert(inputData, argument);
                    arguments.replace(argIndex, argument);
                }
            }
        }

        // ------------ parse optional arguments
        std::vector<SPOperationParameter> parameters = meta->getInputParameters();
        if (inputParamIndex < _expression.parameterCount()) {
            for (int i = 0 ; i < optionals.size(); i++) {
                quint16 optionalIndex = requireds.size() + i;
                if ( !workflow->hasInputAssignment(inputOperation, optionalIndex)) {
                    continue; // implicit or non-existent input
                } else {
                    SPAssignedInputData inputData = workflow->getAssignedInputData({inputOperation, optionalIndex});
                    SPOperationParameter parameter = parameters.at(optionalIndex);
                    QString namedOptional = "%1";/*parameter->term() + "=%1"*/ // TODO named optionals
                    if (inputs.contains(inputData)) {
                        // shared over multiple operations
                        arguments.replace(optionalIndex, namedOptional.arg(inputs.value(inputData)));
                    } else {
                        QString argument;
                        if (inputData->value.size() > 0) {
                            argument = inputData->value;    // pre-filled input value
                        } else {
                            argument = _expression.parm(inputParamIndex).value();
                            inputParamIndex++;
                        }
                        inputs.insert(inputData, argument);
                        arguments.replace(optionalIndex, namedOptional.arg(argument));
                    }
                }
            }
        }

        // ------------ cache to be available when node gets executed

        _inputArgs.insert(inputOperation, arguments);
    }
}

void WorkflowJSONConnector::addGeneratedNames(const OVertex &v, QStringList& names, const IOperationMetaData& meta)
{
    int n = meta->outputParameterCount();
    for(int i=0; i < n; ++i){
        QString name = meta->name() + "_" + QString::number(v) + "_" + QString::number(i);
        names.append(name);
    }
    _nodeExecutionContext[v] = names;
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
    QJsonObject jsonMeta;
    jsonMeta["longname"] = res.name();
    jsonMeta["description"] = res.description();
    jsonMeta["syntax"] = res["syntax"].toString();
    jsonMeta["resource"] = QString("Ilwis");
    jsonMeta["keywords"] = res["keywords"].toString();
    jsonMeta["inputparametercount"] = res["inparameters"].toString();
    jsonMeta["outputparametercount"] = res["outparameters"].toString();
    jsonMeta["final"] = res["final"].toString();

    return jsonMeta;
}

QJsonArray WorkflowJSONConnector::createJSONOperationInputList(Workflow* workflow, const OVertex v) {
    QJsonArray inputs;

    // get the metadata from the operation
    IOperationMetaData meta = workflow->getOperationMetadata(v);
    std::vector<SPOperationParameter> params = meta->getInputParameters();
    foreach (SPOperationParameter parameter, params) {
        QJsonObject input;
        input["url"] = QString("");
        input["local"] = QString("");
        input["value"] = QString("");
        input["name"] = parameter->term();  // weird naming!!
        input["type"] = parameter->name();  // weird naming!!
        input["id"] = QString("");  // filled in in the coming sections
        input["optional"] = parameter->isOptional();
        input["description"] = parameter->description();

        inputs.append(input);
    }

    // pre-assigned values
    for (InputAssignment assignment : workflow->getConstantInputAssignments(v)) {
        SPAssignedInputData inputValue = workflow->getAssignedInputData(assignment);
        QJsonObject input = inputs.at(assignment.second).toObject();
        input["id"] = QString("%1").arg(assignment.second);
        SPOperationParameter parm = meta->inputParameter(assignment.second);
        if (parm) {
            if (hasType(parm->type(), itCOVERAGE)) {
                input["local"] = inputValue->value;
                input["url"] = _config.getWMSGetMapURL(inputValue->value);
            }
            else {
                input["value"] = inputValue->value;
            }
        }
        inputs.replace(assignment.second, input);
    }

    // externals
    QStringList externalInputs = _inputArgs.value(v);
    for (int i = 0 ; i < externalInputs.size() ; i++) {
        auto input = inputs[i].toObject();
        input["id"] = QString("%1").arg(i);
        QString externalInput = externalInputs.at(i);
        QString url = "";
        if ( !externalInput.isEmpty()) {
            int parmIndex = externalInput.indexOf("?input_");
            if (parmIndex == -1)
                url = externalInput;
        }
        SPOperationParameter parm = meta->inputParameter(i);
        if (parm) {
            if (hasType(parm->type(), itCOVERAGE)) {
                input["local"] = url;
                input["url"] = _config.getWMSGetMapURL(url);
            }
            else {
                input["value"] = url;
            }
        }
        inputs.replace(i, input);
    }

    // internal calculation result
    InEdgeIterator ei, ei_end;
    boost::tie(ei, ei_end) = workflow->getInEdges(v);
    for (; ei != ei_end; ei++) {
        OVertex previous = workflow->getSourceOperationNode(*ei);
        IOperationMetaData metaPrev = workflow->getOperationMetadata(previous);
        QStringList names;
        addGeneratedNames(previous, names, metaPrev);
        EdgeProperties edges = workflow->edgeProperties(*ei);
        quint16 inputEdge = edges._inputParameterIndex;
        auto input = inputs[inputEdge].toObject();
        input["id"] = QString("%1").arg(inputEdge);
        input["url"] = names[0];

        inputs.replace(inputEdge, input);
    }

    return inputs;
}

QJsonArray WorkflowJSONConnector::createJSONOperationOutputList(Workflow* workflow, const OVertex v) {
    QJsonArray outputs;

    // get the metadata from the operation
    IOperationMetaData meta = workflow->getOperationMetadata(v);
    std::vector<SPOperationParameter> params = meta->getOutputParameters();
    QStringList names;
    addGeneratedNames(v, names, meta);

    // Identify the parameters that are actual workflow outputs
    std::vector<int> actual(params.size());
    int n = 0;
    std::generate(actual.begin(), actual.end(), [&n]{ return n++; });
    std::pair<OutEdgeIterator, OutEdgeIterator> ei = workflow->getOutEdges(v);
    for (auto &edge = ei.first; edge < ei.second; ++edge) {
        EdgeProperties edgeData = workflow->edgeProperties(*edge);
        actual[edgeData._outputParameterIndex] = -1;    // has internal connection, so remove from actual outputs
    }

    int index = 0;
    foreach (SPOperationParameter parameter, params) {
        QJsonObject output;
        QString url("");
        output["local"] = url;
        output["url"] = url;
        output["value"] = url;
        output["name"] = parameter->term();  // weird naming!!
        output["type"] = parameter->name();  // weird naming!!
        output["id"] = QString("%1").arg(index);
        output["optional"] = parameter->isOptional();
        output["description"] = parameter->description();
        if (actual[index] != -1) { // an actual workflow output parameter
            names[index] += "_out";
        }
        url = names[index];
        if (hasType(parameter->type(), itCOVERAGE)) {
            output["local"] = url;
            output["url"] = _config.getWMSGetMapURL(url);
        }
        else {
            output["value"] = url;
        }
        index++;

        outputs.append(output);
    }

    return outputs;
}

QJsonArray WorkflowJSONConnector::createJSONOperationConnectionList(Workflow *workflow) {
    QJsonArray connections;

    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = workflow->getNodeIterators();
    for (auto iterOper = nodeIterators.first; iterOper < nodeIterators.second; ++iterOper) {
        std::pair<OutEdgeIterator, OutEdgeIterator> edgeIterators = workflow->getOutEdges(*iterOper);

        for (auto &edge = edgeIterators.first; edge < edgeIterators.second; ++edge) {
            OVertex toVertex = workflow->getTargetOperationNode(*edge);
            EdgeProperties edgeData = workflow->edgeProperties(*edge);
            QJsonObject connection;
            connection["fromOperationID"] = (int) *iterOper;
            connection["fromParameterID"] = edgeData._outputParameterIndex;
            connection["toOperationID"] = (int) toVertex;
            connection["toParameterID"] = edgeData._inputParameterIndex;

            connections.append(connection);
        }
    }

    return connections;
}
