#ifndef STREAMCONNECTORV1_H
#define STREAMCONNECTORV1_H

namespace Ilwis {
class DataDefinition;

namespace Stream {

class StreamConnectorV1 : public DataInterface
{
public:
    StreamConnectorV1(QDataStream &stream);

    virtual ~StreamConnectorV1();

    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    bool loadData(IlwisObject*, const IOOptions& options = IOOptions()) { return false; }
    bool dataIsLoaded() const { return false; }
    bool store(IlwisObject *obj, int );
    //DataInterface* clone() const { return 0;}

protected:
    QDataStream& _stream;

    bool storeDataDefintion(const DataDefinition& def, QDataStream& stream,  int options) const;
    bool loadDataDefinition(DataDefinition& def, QDataStream& stream, const Ilwis::IOOptions &options);
};
}
}

#endif // STREAMCONNECTORV1_H
