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
    QString type() const;
    virtual IlwisObject *create() const;
    bool loadBinaryData(IlwisObject *);

    static ConnectorInterface *create(const Ilwis::Resource &item, bool load);
private:
    ColumnDefinition getKeyColumn();
    ColumnDefinition makeColumn(const QString &colName, quint64 index);
    QString valueType2DataType(IlwisTypes ty);
    QHash<QString, RawConverter> _converters;
};
}
}

#endif // TABLECONNECTOR_H
