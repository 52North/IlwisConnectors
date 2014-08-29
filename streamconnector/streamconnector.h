#ifndef StreamConnector_H
#define StreamConnector_H

namespace Ilwis {
namespace Stream {

class VersionedSerializer;

class StreamConnector : public IlwisObjectConnector
{
public:
    StreamConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    virtual ~StreamConnector();

    bool loadMetaData(IlwisObject* object, const IOOptions&);
    bool loadData(IlwisObject *data, const IOOptions &options);

    QString provider() const;
    bool needFlush() const;
    void flush(bool last);
    static ConnectorInterface *create(const Resource &resource, bool load, const IOOptions &options);

private:
    std::unique_ptr<VersionedSerializer> _versionedConnector;
    std::unique_ptr<QIODevice> _datasource;
    QByteArray _bytes;


    bool store(IlwisObject *obj, int options);
    bool openSource(bool reading);
    IlwisObject *create() const;
};

}
}

#endif // StreamConnector_H
