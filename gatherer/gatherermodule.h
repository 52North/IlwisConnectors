#ifndef GATHERERMODULE_H
#define GATHERERMODULE_H

#include <QObject>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {
namespace Gatherer {

class GathererModule : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit GathererModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    virtual void finalizePreparation();

    Q_PLUGIN_METADATA(IID "n52.ilwis.gatherer" FILE  "gatherer.json")

signals:

public slots:

};
}
}

#endif // ILWIS3MODULE_H
