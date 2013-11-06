#include "../../IlwisCore/core/kernel.h"

#include "ilwis.h"
#include "issuelogger.h"

#include <QtCore/QCoreApplication>
#include <QString>
#include <QFileInfo>

static pythonapi::IssueLogger* logger;

bool pythonapi::initIlwisObjects(){
    int argc = 0;
    char* argv[0];
    QCoreApplication app(argc, argv);
    bool ret = Ilwis::initIlwis(QFileInfo("C:/Python33/Lib/site-packages/ilwisobjects.conf"));
    logger = new pythonapi::IssueLogger();
    QObject::connect(Ilwis::kernel()->issues().data(),SIGNAL(ilwiserrormessage(QString)),logger,SLOT(ilwiserrormessage(QString)));
    return ret;
}
