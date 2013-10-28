#include "../../IlwisCore/core/kernel.h"

#include "ilwis.h"

#include <QtCore/QCoreApplication>
#include <QString>
#include <QFileInfo>

bool pythonapi::initIlwisObjects(){
    int argc = 0;
    char* argv[0];
    QCoreApplication app(argc, argv);
    return Ilwis::initIlwis(QFileInfo("./Lib/site-packages/ilwisobjects.config"));
}
