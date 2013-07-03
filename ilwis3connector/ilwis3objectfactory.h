#ifndef ILWIS3OBJECTFACTORY_H
#define ILWIS3OBJECTFACTORY_H

namespace Ilwis {
class Ilwis3ObjectFactory : public IlwisObjectFactory
{
public:
    Ilwis3ObjectFactory();

    IlwisObject *create(const Ilwis::Resource &item) const;
    bool canUse(const Resource &resource) const;
    bool prepare();
private:
    void loadIlwis3Aliasses();
    void insertFile(const QString &filename, QSqlQuery &sqlPublic);
    bool fillEllipsoidRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillDatumRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillProjectionRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillDomainRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool doQuery(QString& query, QSqlQuery &sqlPublic);

    QHash<IlwisTypes,CheckUsage> _types;
};
}

#endif // ILWIS3OBJECTFACTORY_H
