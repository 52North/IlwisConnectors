#ifndef WORKFLOWSERIALIZERV1_H
#define WORKFLOWSERIALIZERV1_H

namespace Ilwis {
namespace Stream {

class WorkflowSerializerV1 : public VersionedSerializer
{
public:
    WorkflowSerializerV1(QDataStream& stream);

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // WORKFLOWSERIALIZERV1_H
