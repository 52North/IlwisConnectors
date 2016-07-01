#ifndef WORKFLOWJSONCONNECTOR_H
#define WORKFLOWJSONCONNECTOR_H

#include <QBuffer>

namespace Ilwis {

class Workflow;

namespace WorkflowConnector {

    class WorkflowJSONConnector : public IlwisObjectConnector
    {
    public:
        WorkflowJSONConnector(const Ilwis::Resource &resource, bool,const IOOptions& options=IOOptions() );

        bool loadMetaData(IlwisObject*, const IOOptions & );
        bool loadData(IlwisObject*, const IOOptions& options = IOOptions());
        bool store(IlwisObject *, const IOOptions& options = IOOptions() );
        QString type() const;
        bool isReadOnly() const;
        Ilwis::IlwisObject *create() const;
        QString provider() const;
        QVariant getProperty(const QString& ) const;

        static ConnectorInterface *create(const Ilwis::Resource &resource,bool load = true,const IOOptions& options=IOOptions());

    private:
        QJsonObject createJSONWorkflow(const Resource & res);
        QJsonObject createJSONWorkflowMetaData(const Resource& res);
        QJsonObject createJSONOperationMetadata(const Resource &res);
        QJsonObject createJSONOperationList(const Resource &res);
        QJsonArray createJSONOperationInputList(Workflow* workflow, const OVertex v);
        QJsonArray createJSONOperationOutputList(Ilwis::Workflow *workflow, const Ilwis::OVertex v);
        QJsonArray createJSONOperationConnectionList(const Resource &res);
        bool openTarget();

        std::unique_ptr<QIODevice> _datasource;

        QByteArray _data;
        QMap<OVertex, QStringList> _nodeExecutionContext;   // contains a list of completed operations
        QMap<OVertex, QStringList> _inputArgs;
        OperationExpression _expression;
        QStringList _outputNames;

        bool reverseFollowPath(Workflow *workflow, const OVertex &v, QStringList &names, QStringList &script,int order=-1);
        bool doInputEdges(InEdgeIterator &ei, const InEdgeIterator &ei_end, Ilwis::Workflow *workflow, const OVertex &v, QStringList &arguments, QStringList &script);
        void executeInputNode(Workflow *workflow, const OVertex &v, QStringList &names, QStringList &script);
        void parseInputNodeArguments(Ilwis::Workflow *workflow, const QList<OVertex> &inputNodes);
        void addGeneratedNames(const OVertex &v, QStringList& names, const Ilwis::IOperationMetaData &meta);
        QString createArgumentList(const Ilwis::IOperationMetaData &meta, const QStringList &arguments);
    };

}
}

#endif // WORKFLOWJSONCONNECTOR_H
