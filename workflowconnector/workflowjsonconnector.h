#ifndef WORKFLOWJSONCONNECTOR_H
#define WORKFLOWJSONCONNECTOR_H

namespace Ilwis {
namespace WorkflowConnector {
class WorkflowJSONConnector : public IlwisObjectConnector
{
public:
    WorkflowJSONConnector(const Ilwis::Resource &resource, bool,const IOOptions& options=IOOptions() );

    bool loadMetaData(IlwisObject*, const IOOptions & );
    bool loadData(IlwisObject*, const IOOptions& options = IOOptions());
    bool store(IlwisObject *, const IOOptions& options = IOOptions() );
    QString format() const;
    bool isReadOnly() const;
private:
    QJsonObject createJSONOperationMetaData(const Resource& res);
};
}
}

#endif // WORKFLOWJSONCONNECTOR_H
