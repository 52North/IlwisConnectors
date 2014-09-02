#ifndef VERSIONEDSERIALIZER_H
#define VERSIONEDSERIALIZER_H

namespace Ilwis {
class DataDefinition;

namespace Stream {

class StreamConnector;

class VersionedSerializer : public DataInterface
{
public:
    VersionedSerializer(QDataStream &stream);

    virtual ~VersionedSerializer();

    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    bool loadData(IlwisObject*, const IOOptions& options = IOOptions()) { return false; }
    bool dataIsLoaded() const { return false; }
    bool store(IlwisObject *obj, int );
    virtual bool storeData(IlwisObject *obj, int ) { return true;} // not mandatory to implement this, so empty is ok
    void connector(StreamConnector *streamconnector);

protected:
    QDataStream& _stream;
    StreamConnector *_streamconnector = 0;

    bool storeDataDefintion(const DataDefinition& def, QDataStream& stream,  int options) const;
    bool loadDataDefinition(DataDefinition& def, QDataStream& stream, const Ilwis::IOOptions &options);
};
}
}

#endif // STREAMCONNECTORV1_H
