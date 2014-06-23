#ifndef WCSModule_H
#define WCSModule_H

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"


namespace Ilwis {
namespace Wcs {

class WCSModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit WCSModule(QObject *parent = 0);
    ~WCSModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& commands) const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.wcsconnector" FILE  "wcsconnector.json")

private:


signals:
};
}
}

#endif // WCSModule_H
