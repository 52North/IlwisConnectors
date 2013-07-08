#ifndef ILWIS3CATALOGCONNECTOR_H
#define ILWIS3CATALOGCONNECTOR_H

namespace Ilwis {

namespace Ilwis3 {

class Ilwis3CatalogConnector : public FileCatalogConnector
{
public:
    Ilwis3CatalogConnector(const Ilwis::Resource &item);

    virtual bool loadItems();
    bool canUse(const QUrl& res) const;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &item, bool);
private:
    void registerNames(const QString &name, QHash<QString, quint64> &names);
};
}
}

#endif // ILWIS3CATALOGCONNECTOR_H
