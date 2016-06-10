#ifndef WORKFLOWJSONCONNECTOR_H
#define WORKFLOWJSONCONNECTOR_H

#include "workflow/workflow.h"

namespace Ilwis {
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
        QJsonArray createJSONOperationInputList(const IOperationMetaData &meta);
        QJsonArray createJSONOperationOutputList(const IOperationMetaData &meta);
        QJsonArray createJSONOperationConnectionList(const Resource &res);
        bool openTarget();

        std::unique_ptr<QIODevice> _datasource;
        QByteArray _data;
    };

}
}

#endif // WORKFLOWJSONCONNECTOR_H
