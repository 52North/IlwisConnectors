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

bool WorkflowSerializerV1::storeNode(SPWorkFlowNode node, const IOOptions &options){
    _stream << node->name();
    _stream << node->description();
    _stream << node->id();
    IOperationMetaData op = node->operation();
    _stream << (op.isValid() ? op->id() : i64UNDEF);
    _stream << node->type();
    if ( node->type() == "conditionnode"){
        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
        int sz = condition->subnodes("tests").size();
        _stream << sz;
        for(int t=0; t < sz; ++t){
            WorkFlowCondition::Test test = condition->test(t)    ;
            _stream << (int)test._pre;
            _stream << (int)test._post;
            _stream << test._operation->id();
        }
        auto operations = condition->subnodes("operations")    ;
        _stream << (int)operations.size();
        for(int o=0; o < operations.size(); ++o){
            storeNode(operations[o]);
        }
    }
    node->box().store(_stream);
    _stream << node->inputCount();

    for(int i = 0; i < node->inputCount(); ++i){
        WorkFlowParameter& wp = node->inputRef(i);
        if (wp.isValid()){
            _stream << wp.id();
            _stream << wp.name();
            _stream << wp.description();

            _stream << wp.label();
            if ( wp.outputParameterIndex() != iUNDEF)
                _stream << wp.inputLink()->id();
            else
                _stream << i64UNDEF;
            _stream << wp.outputParameterIndex();
            _stream << wp.valueType();
            _stream << (qint32)wp.state();
            _stream << wp.attachement(true);
            _stream << wp.attachement(false);
            _stream << wp.syntax();
            if(!VersionedSerializer::store(wp.value(), wp.valueType(), options))
                return false;
        }else
            _stream << i64UNDEF;
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

    Workflow::ExecutionOrder order = workflow->executionOrder();
    std::vector<SPWorkFlowNode> nodes = order._independentOrder;
    std::reverse(nodes.begin(),nodes.end());

    _stream << (int)nodes.size();
    for(SPWorkFlowNode node : nodes){
        storeNode(node, options);
    }
    _stream << (int)order._dependentOrder.size();
    for(auto item : order._dependentOrder){
        nodes = item.second;
        _stream << item.first;
        _stream << (int)nodes.size();
        for(const SPWorkFlowNode& node : nodes){
            storeNode(node);
        }
    }
    _dataLoaded = true;
    return true;
}

void WorkflowSerializerV1::loadNode(SPWorkFlowNode& node,Workflow *workflow, const IOOptions &options){

    QString nm, type;
    _stream >> nm;
    QString ds;
    _stream >> ds;
    quint64 nodeid, operationid;
    _stream >> nodeid;
    _stream >> operationid;
    _stream >> type;
    if ( type == "operationnode"){
        auto opNode = new OperationNode(nm, ds,nodeid);
        opNode->operationid(operationid);
        node.reset(opNode);
    }else if ( type == "conditionnode"){
        auto cnode = new WorkFlowCondition();
        int pre,post;
        quint64 operationid;
        int ocount;
        _stream >> ocount;
        for(int t=0; t < ocount; ++t){
            _stream >> pre;
            _stream >> post;
            _stream >> operationid;
            SPWorkFlowNode operNode(new OperationNode(operationid));
            cnode->addTest(operNode, (LogicalOperator) pre,(LogicalOperator) post);
        }
        _stream >> ocount;
        for(int o=0; o < ocount; ++o){
            SPWorkFlowNode operationNode;
            loadNode(operationNode, workflow);
        }
        node.reset(cnode);
    }else if ( type == "junctionnode"){
        auto cnode = new Junction();
        node.reset(cnode);

    }
    BoundingBox box;
    box.load(_stream);
    node->box(box);
    int parmCount;
    _stream >> parmCount;

    for(int j=0; j < parmCount; ++j){
        quint64 parmid;
        _stream >> parmid;
        if ( parmid == i64UNDEF)
            continue;
        _stream >> nm;
        _stream >> ds;

        QString label;
        _stream >> label;


        qint32 outParmindex;
        _stream >> nodeid;
        _stream >> outParmindex;
        SPWorkFlowNode prevNode;
        if ( nodeid != i64UNDEF)
            prevNode = workflow->nodeById(nodeid);
        IlwisTypes tp;
        _stream >> tp;
        int state;
        _stream >> state;
        WorkFlowParameter wp(parmid, node->id(), nm,ds);
        if ( prevNode)
            wp.inputLink(prevNode, outParmindex);
        int rctIndex;
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
        node->addInput(wp);
    }

}

bool WorkflowSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);
    if (!OperationMetadataSerializerV1::loadMetaData(obj, options))
        return false;
    double scale;
    int translationx, translationy;
    _stream >> scale;
    _stream >> translationx;
    _stream >> translationy;
    workflow->scale(scale);
    workflow->translation(translationx, translationy);
    int sz;
    _stream >> sz;
    for(int i = 0; i < sz ; ++i){
        SPWorkFlowNode node;
        loadNode(node, workflow);

        workflow->addNode(node);
    }

    return true;
}
