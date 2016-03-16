#ifndef VERSIONEDSERIALIZER_H
#define VERSIONEDSERIALIZER_H

namespace Ilwis {
class DataDefinition;

namespace Stream {

class StreamConnector;
class RawConverter;

class VersionedSerializer : public DataInterface
{
public:
    VersionedSerializer(QDataStream &stream);

    virtual ~VersionedSerializer();

    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    bool loadData(IlwisObject*, const IOOptions& options = IOOptions()) { return false; }
    virtual bool loadItems(std::vector<Resource>& ) ;
    bool dataIsLoaded() const ;
    bool store(IlwisObject *obj, const IOOptions& options = IOOptions() );
    virtual bool storeData(IlwisObject *obj, const IOOptions& options = IOOptions() ) { return true;} // not mandatory to implement this, so empty is ok
    virtual quint32 loadGridBlock(IlwisObject *data, quint32 block, QByteArray &blockdata, const RawConverter& converter, const IOOptions &) { return 0;}
    void connector(StreamConnector *streamconnector);

protected:
    QDataStream& _stream;
    StreamConnector *_streamconnector = 0;
    bool _dataLoaded = false;

    bool storeDataDefintion(const DataDefinition& def, QDataStream& stream,  const IOOptions& options = IOOptions()) const;
    bool loadDataDefinition(DataDefinition& def, QDataStream& stream, const Ilwis::IOOptions &options);
};
}
}

#endif // STREAMCONNECTORV1_H
