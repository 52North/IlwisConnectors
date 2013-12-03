#ifndef ILWIS3CONNECTOR_H
#define ILWIS3CONNECTOR_H

#include <QScopedPointer>
#include "Ilwis3Connector_global.h"


namespace Ilwis {

namespace Ilwis3 {

class ILWIS3CONNECTORSHARED_EXPORT Ilwis3Connector : public Ilwis::IlwisObjectConnector {
public:
    Ilwis3Connector(const Ilwis::Resource &resource, bool load=true);
    //static IlwisTypes type(const QString &name);
    static IlwisTypes ilwisType(const QString &name);
    static QString name2Code(const QString &name, const QString &type);
    static QString code2name(const QString &code, const QString &type);
    QString provider() const;
    virtual bool storeBinaryData(IlwisObject* ) { return false; }
    virtual bool storeMetaData(IlwisObject* )  { return false; }
    QString unquote(const QString &name) const;

    bool store(IlwisObject *, int storemode);

protected:
    bool willStore(const Ilwis::IlwisObject *obj) const;
    QString suffix(IlwisTypes type) const;
    QUrl resolve(const Resource &resource) const;
    bool isSystemObject(const QString &filename);
    bool loadMetaData(IlwisObject* data);
    bool storeMetaData(const IlwisObject* data, IlwisTypes type) const;
    QString adjustPath(const QUrl& container, const QString& filename) const;
    QString unquote(const QString& name);


    QString ilwis3ClassName(IlwisTypes type) const;
    QString datum2Code(const QString& name, const QString& area) const;
    QString noExt(const QString& name);
    QString filename2FullPath(const QString &name, const Resource &owner=Resource()) const;

    mutable ODF _odf;

    QUrl makeUrl(const QString &path, const QString &name=sUNDEF);
};
}

}

#endif // ILWIS3CONNECTOR_H
