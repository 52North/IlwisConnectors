#ifndef TABLECONNECTOR_H
#define TABLECONNECTOR_H

namespace Ilwis {
namespace Ilwis3{

class RawConverter;

class TableConnector : public Ilwis3Connector
{
public:
    TableConnector(const Ilwis::Resource &item, bool load);
    bool loadMetaData(IlwisObject* data);
    bool storeMetaData(Ilwis::IlwisObject *obj);
    QString type() const;
    virtual IlwisObject *create() const;
    bool loadBinaryData(IlwisObject *);
    bool storeBinaryData(IlwisObject* obj);

    static ConnectorInterface *create(const Ilwis::Resource &item, bool load);
    static bool storeTable(const ITable& tbl);
    void attributeDomain(const QString& attdom);
private:
    ColumnDefinition getKeyColumn();
    ColumnDefinition makeColumn(const QString &colName, quint64 index);
    QString valueType2DataType(IlwisTypes ty);
    QString getDomainName(const IDomain &dom, bool& isSystem);

    QHash<QString, RawConverter> _converters;
    QString _attributeDomain;
};
}
}

#endif // TABLECONNECTOR_H
