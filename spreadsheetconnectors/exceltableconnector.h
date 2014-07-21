#ifndef EXCELTABLECONNECTOR_H
#define EXCELTABLECONNECTOR_H

namespace Ilwis {
namespace SpreadSheetConnectors {


class ExcelTableConnector : public IlwisObjectConnector
{
public:
    ExcelTableConnector(const Ilwis::Resource &resource, bool load,const PrepareOptions& options=PrepareOptions());
    bool loadMetaData(IlwisObject* data, const PrepareOptions&options);
    bool storeMetaData(Ilwis::IlwisObject *obj);
    QString type() const;
    virtual IlwisObject *create() const;
    bool loadData(IlwisObject *, const LoadOptions& options = LoadOptions());
    bool storeBinaryData(IlwisObject* obj);
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const PrepareOptions& options=PrepareOptions());
};
}
}

#endif // EXCELTABLECONNECTOR_H
