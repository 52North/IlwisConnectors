#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "operationmetadataserializerv1.h"
#include "workflowserializerv1.h"
#include "workflow.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *WorkflowSerializerV1::create(QDataStream& stream)
{
    return new WorkflowSerializerV1(stream);
}

WorkflowSerializerV1::WorkflowSerializerV1(QDataStream &stream) : OperationMetadataSerializerV1(stream)
{
}

bool WorkflowSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);
    if (!OperationMetadataSerializerV1::store(obj, options))
        return false;

    // Input asignments
    QMap<InputAssignment, SPAssignedInputData> asignments = workflow->getAllInputAssignments();

    int sz = asignments.size();
    _stream << sz;
    for (const auto &key: asignments.keys()) {
        _stream << (int)key.first;
        _stream << key.second;

        const SPAssignedInputData &asignment = asignments.value(key);
        _stream << asignment->inputName;
        _stream << asignment->value;
    }

    // Nodes
    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = workflow->getNodeIterators();
    int size = nodeIterators.second - nodeIterators.first;

    _stream << size;
    for (auto iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {

        NodeProperties nodeData = workflow->nodeProperties(*iter);
        _stream << (quint32)*iter;
        _stream << nodeData._resourceProvider;
        _stream << nodeData._syntax;
        _stream << nodeData._x;
        _stream << nodeData._y;
        _stream << nodeData._id;
        qDebug() << nodeData._x << nodeData._y << nodeData._syntax;
    }

    // Edges
    for (auto iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {
        std::pair<OutEdgeIterator, OutEdgeIterator> edgeIterators = workflow->getOutEdges(*iter);
        size = edgeIterators.second - edgeIterators.first;
        _stream << size;
        for (auto &iter2 = edgeIterators.first; iter2 < edgeIterators.second; ++iter2) {
            OVertex toVertex = workflow->getTargetOperationNode(*iter2);
            EdgeProperties edgeData = workflow->edgeProperties(*iter2);
            _stream << (int)toVertex;
            _stream << edgeData._outputParameterIndex;
            _stream << edgeData._inputParameterIndex;
            _stream << edgeData._outputRectangleIndex;
            _stream << edgeData._inputRectangleIndex;
        }
    }
    _dataLoaded = true;
    return true;
}

bool WorkflowSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);
    if (!OperationMetadataSerializerV1::loadMetaData(obj, options))
        return false;
    int size;
    _stream >> size;
    for (int i = 0; i < size; ++i) {
        int vertex;
        int parameterIndex;
        AssignedInputData *properties = new AssignedInputData();

        _stream >> vertex;
        _stream >> parameterIndex;
        _stream >> properties->inputName;
        _stream >> properties->value;

        workflow->assignInputData(
            {vertex, parameterIndex},
            SPAssignedInputData(properties)
        );
    }

    int nodesSize;
    _stream >> nodesSize;
    QMap<OVertex, OVertex> vertexMapping;
    for (int i = 0; i < nodesSize; ++i) {
        quint32 currentVertex;
        QString provider;
        QString syntax;
        qint32 x;
        qint32 y;
        quint64 id;

        _stream >> currentVertex;
        _stream >> provider;
        _stream >> syntax;
        _stream >> x;
        _stream >> y;
        _stream >> id;
        vertexMapping.insert(currentVertex, workflow->addOperation({syntax, provider, x, y,id}));
    }
    for (const auto &fromVertex : vertexMapping.keys()) {
        int edgesSize;
        _stream >> edgesSize;
        for (int i = 0; i < edgesSize; ++i) {
            int toVertex;
            int outParmIndex;
            int inParmIndex;
            int outRectIndex;
            int inRectIndex;

            _stream >> toVertex; //TODO cast to OVertex
            toVertex = vertexMapping.value(toVertex);
            _stream >> outParmIndex;
            _stream >> inParmIndex;
            _stream >> outRectIndex;
            _stream >> inRectIndex;
            workflow->addOperationFlow(fromVertex, toVertex, {outParmIndex, inParmIndex, outRectIndex, inRectIndex});
        }
    }
    return true;
}
