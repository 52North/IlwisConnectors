#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "operationmetadataserializerv1.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "junctionNode.h"
#include "operationnode.h"
#include "conditionNode.h"
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

void WorkflowSerializerV1::storeNodeLinks(const SPWorkFlowNode& node) {
    if ( node->type() == "conditionnode"){
        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
        qint32 sz = condition->subnodes("tests").size();
        _stream << sz;
        for(qint32 t=0; t < sz; ++t){
            WorkFlowCondition::Test test = condition->test(t)    ;
           storeNodeLinks(test._operation);
        }
        auto operations = condition->subnodes("operations")    ;
        for(qint32 o=0; o < operations.size(); ++o){
            storeNodeLinks(operations[o]);
        }
    }
    int count = node->inputCount();
    if ( node->type() == "junctionnode")
        // junctions have 3 parameters; but inputcount returns only 1 as the link to the condition is the only explicit parameter;
        //links to operations are implicit. Needed for saving though so we overrule here
        count = 3;

    _stream << count;

    for(qint32 i = 0; i < count; ++i){
        WorkFlowParameter& wp = node->inputRef(i);
        _stream << wp.outputParameterIndex();
        if (wp.isValid()){
            if ( wp.inputLink())
                _stream << wp.inputLink()->id();
            else
                _stream << i64UNDEF;
        }else
            _stream << i64UNDEF;
    }
}

bool WorkflowSerializerV1::storeNode(const SPWorkFlowNode& node, const IOOptions &options){
    _stream << node->name();
    _stream << node->description();
    _stream << node->id();
    IOperationMetaData op = node->operation();
    _stream << (op.isValid() ? op->id() : i64UNDEF);
    _stream << node->type();
    _stream << (qint32)node->collapsed();
    if ( node->type() == "conditionnode"){
        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
        qint32 sz = condition->subnodes("tests").size();
        _stream << sz;
        for(qint32 t=0; t < sz; ++t){
            WorkFlowCondition::Test test = condition->test(t)    ;
            _stream << (int)test._pre;
            _stream << (int)test._post;
           storeNode(test._operation);
        }
        auto operations = condition->subnodes("operations")    ;
        _stream << (int)operations.size();
        for(qint32 o=0; o < operations.size(); ++o){
            storeNode(operations[o]);
        }
    }
    node->box().store(_stream);

    int count = node->inputCount();
    if ( node->type() == "junctionnode")
        // junctions have 3 parameters; but inputcount returns only 1 as the link to the condition is the only explicit parameter;
        //links to operations are implicit. Needed for saving though so we overrule here
        count = 3;
    _stream << count;



    for(qint32 i = 0; i < count; ++i){
        WorkFlowParameter& wp = node->inputRef(i);
        _stream << wp.id();
        _stream << wp.name();
        _stream << wp.description();
        _stream << wp.label();
        _stream << wp.valueType();
        _stream << (qint32)wp.state();
        _stream << wp.attachement(true);
        _stream << wp.attachement(false);
        _stream << wp.syntax();
        if(!VersionedSerializer::store(wp.value(), wp.valueType(), options))
            return false;
    }
    return true;
}

bool WorkflowSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);
    if (!OperationMetadataSerializerV1::store(obj, options))
        return false;


    _stream << workflow->scale();
    _stream << workflow->translation().first;
    _stream << workflow->translation().second;

    const std::vector<SPWorkFlowNode>& graph = workflow->graph();

    _stream << (int)graph.size();
    for(const SPWorkFlowNode node : graph){
        storeNode(node, options);
    }
    for(const SPWorkFlowNode& node : graph){
        storeNodeLinks(node);
    }
    _dataLoaded = true;
    return true;
}

void WorkflowSerializerV1::loadNodeLinks(SPWorkFlowNode& node,Workflow *workflow){

     if ( node->type() == "conditionnode"){
        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
        qint32 ocount;
        _stream >> ocount;
        for(qint32 t=0; t < ocount; ++t){
            WorkFlowCondition::Test test = condition->test(t);
            SPWorkFlowNode operationNode;
            loadNodeLinks(test._operation, workflow);
        }
        auto subnodes = condition->subnodes("operations");
        for(SPWorkFlowNode& operationNode : subnodes){
            loadNodeLinks(operationNode, workflow);
        }
    }
    qint32 parmCount;
    _stream >> parmCount;

    for(qint32 j=0; j < parmCount; ++j){
        qint32 outParmindex;
        NodeId nodeid;
        _stream >> outParmindex;
        _stream >> nodeid;
        SPWorkFlowNode prevNode;
        if ( nodeid != i64UNDEF)
            prevNode = workflow->nodeById(nodeid);
        if ( prevNode)
            node->inputRef(j).inputLink(prevNode, outParmindex);
    }
}

void WorkflowSerializerV1::loadNode(SPWorkFlowNode& node,Workflow *workflow, const IOOptions &options){

    QString nm, type;
    qint32 collapsed;
    _stream >> nm;
    QString ds;
    _stream >> ds;
    quint64 nodeid, operationid;
    _stream >> nodeid;
    _stream >> operationid;
    _stream >> type;
    _stream >> collapsed;
    if ( type == "operationnode"){
        auto opNode = new OperationNode(nm, ds,nodeid);
        opNode->operationid(operationid);
        node.reset(opNode);
    }else if ( type == "conditionnode"){
        auto cnode = new WorkFlowCondition();
        cnode->nodeId(nodeid);
        node.reset(cnode);
        qint32 pre,post;
        qint32 ocount;
        _stream >> ocount;
        for(qint32 t=0; t < ocount; ++t){
            _stream >> pre;
            _stream >> post;
            SPWorkFlowNode operationNode;
            loadNode(operationNode, workflow);
            cnode->addTest(operationNode, (LogicalOperator) pre,(LogicalOperator) post);
        }
        _stream >> ocount;
        for(qint32 o=0; o < ocount; ++o){
            SPWorkFlowNode operationNode;
            loadNode(operationNode, workflow);
            cnode->addSubNode(operationNode,"operations");
            operationNode->owner(node);
        }

    }else if ( type == "junctionnode"){
        auto cnode = new Junction();
        cnode->nodeId(nodeid);
        node.reset(cnode);

    }
    if (!node){
        throw ErrorObject(TR("Stored workflow definition is invalid"));
    }
    node->collapsed(collapsed > 0);
    BoundingBox box;
    box.load(_stream);
    node->box(box);
    qint32 parmCount;
    _stream >> parmCount;

    for(qint32 j=0; j < parmCount; ++j){
        quint64 parmid;

        _stream >> parmid;
        _stream >> nm;
        _stream >> ds;

        QString label;
        _stream >> label;

        IlwisTypes tp;
        _stream >> tp;
        qint32 state;
        _stream >> state;
        WorkFlowParameter wp(parmid, node->id(), nm,ds);

        qint32 rctIndex;
        _stream >> rctIndex;
        wp.attachement(rctIndex, true);
        _stream >> rctIndex;
        wp.attachement(rctIndex, false);
        QString syntax;
        _stream >> syntax;
        wp.addSyntax(syntax);
        QString v;
        IOOptions opt = options;
        opt.addOption("mustexist", true);
        if(!VersionedSerializer::loadMetaData(opt, tp, v))
            return;
        wp.value(v, tp);
        node->addInput(wp,j);
    }
}

bool WorkflowSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);
    if (!OperationMetadataSerializerV1::loadMetaData(obj, options))
        return false;
    double scale;
    qint32 translationx, translationy;
    _stream >> scale;
    _stream >> translationx;
    _stream >> translationy;
    workflow->scale(scale);
    workflow->translation(translationx, translationy);
    qint32 sz;
    _stream >> sz;
    std::vector<SPWorkFlowNode> independenNodes;
    for(qint32 i = 0; i < sz ; ++i){
        SPWorkFlowNode node;
        loadNode(node, workflow);

        workflow->addNode(node);
        independenNodes.push_back(node);
    }
    for(SPWorkFlowNode& node : independenNodes){
        loadNodeLinks(node, workflow);
    }

    workflow->updateIdCounter();

    return true;
}
