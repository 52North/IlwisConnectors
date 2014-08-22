#ifndef StreamConnector_H
#define StreamConnector_H

namespace Ilwis {
namespace Stream {

class StreamConnector : public IlwisObjectConnector
{
public:
    StreamConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    virtual ~StreamConnector();

    bool loadMetaData(IlwisObject* data,const IOOptions&);

    QString provider() const;
    static ConnectorInterface *create(const Resource &resource, bool load, const IOOptions &options);
private:
    std::unique_ptr<DataInterface> _versionedConnector;
    std::unique_ptr<QIODevice> _datasource;

    bool store(IlwisObject *obj, int options);
    bool openSource(bool reading);
    IlwisObject *create() const;
};
}
}

#endif // StreamConnector_H
