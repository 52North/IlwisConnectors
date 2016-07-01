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
#include "workflow/workflow.h"
#include "workflow/workflowoperationimplementation.h"
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

    QJsonObject single = createJSONWorkflow(workflow->resource());
    single["metadata"] = createJSONWorkflowMetaData(workflow->resource());

    // deal with all the operations
    QJsonArray operations;
    int operID = 0;

    QList<OVertex> inputOperations = workflow->getNodesWithExternalInput();
    parseInputNodeArguments(workflow, inputOperations);

/*
    QList<OVertex> outputOperations = workflow->getNodesWithExternalOutputs();
    for (int operIndex = 0; operIndex < outputOperations.size(); ++operIndex) {
        QJsonObject operation;
        QJsonObject obj = createJSONOperationMetadata(operIndex);
        operation["id"] = QString::number(operID);
        operation["metadata"] = obj;
        operations.append(operation);

        bool ok = reverseFollowPath(workflow, outputOperations[operIndex], completedOutput, operations, operIndex);
        if (!ok)
        {
            ERROR0("Workflow deduction failed, probably incomplete!");
            return false;
        }
    }
    // TODO: replace unknown inputs, outputs

    return true;
*/
    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = workflow->getNodeIterators();
//    int size = nodeIterators.second - nodeIterators.first;

    for (auto iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {
        OVertex v = *iter;
        NodeProperties nodeData = workflow->nodeProperties(*iter);
        Resource res = mastercatalog()->id2Resource(nodeData._operationid);
        IOperationMetaData metadata;
        metadata.prepare(res);
        if (res.isValid()) {
            QJsonObject operation;
            QJsonObject obj = createJSONOperationMetadata(res);
            operation["id"] = QString::number(operID);
            operation["metadata"] = obj;
            operation["inputs"] = createJSONOperationInputList(workflow, v);
            operation["outputs"] = createJSONOperationOutputList(workflow, v);
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

/*
 * // copy of python connector
bool WorkflowJSONConnector::store(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);

    if ( !options.contains("expression"))
        return false;

    auto expression = options["expression"].value<OperationExpression>();
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

    QStringList names, script;
    QList<OVertex> inputNodes = workflow->getNodesWithExternalInput();
    parseInputNodeArguments(workflow, inputNodes);

    QList<OVertex> outputNodes = workflow->getNodesWithExternalOutputs();
    for (int i = 0; i<outputNodes.size(); ++i) {
        bool ok = reverseFollowPath(workflow, outputNodes[i], names, script,i);
        if (!ok) {
            ERROR0("Workflow deduction failed, probably incomplete!");
            return false;
        }

   }

    if (!openTarget())
        return false;
    QTextStream stream(_datasource.get());
    QString txt = script.join("\n");
    int parmIndex;
    int count = 1;
    while((parmIndex = txt.indexOf("?input_")) != -1){
        QString front = txt.left(parmIndex);
        QString back = txt.mid(parmIndex + 8);
        txt = front + "argv[" + QString::number(count) + "]" + back;
        ++count;
    }
    while((parmIndex = txt.indexOf("?output_")) != -1){
        QString front = txt.left(parmIndex);
        QString back = txt.mid(parmIndex + 9);
        txt = front + "argv[" + QString::number(count) + "]" + back;
        ++count;
    }
    stream << txt;
    stream.device()->close();
    return true;

}
*/
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
        input["value"] = QString("");
        input["name"] = parameter->term();  // weird naming!!
        input["type"] = parameter->name();  // weird naming!!
        input["id"] = QString("");
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
            if (hasType(parm->type(), itILWISOBJECT)) {
                input["url"] = inputValue->value;
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
            if (hasType(parm->type(), itILWISOBJECT)) {
                input["url"] = url;
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
    foreach (SPOperationParameter parameter, params) {
        QJsonObject output;
        output["url"] = QString("");
        output["value"] = QString("");
        output["name"] = parameter->term();  // weird naming!!
        output["type"] = parameter->name();  // weird naming!!
        output["id"] = QString("");
        output["optional"] = parameter->isOptional();
        output["description"] = parameter->description();

        outputs.append(output);
    }

    return outputs;
}

QJsonArray WorkflowJSONConnector::createJSONOperationConnectionList(const Resource &res) {
    QJsonArray connections;

    return connections;
}

