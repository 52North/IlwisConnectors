#include "../../IlwisCore/core/kernel.h"

#include "pythonapi_extension.h"
#include "pythonapi_qissuelogger.h"
#include "pythonapi_error.h"

#include <QtCore/QCoreApplication>
#include <QString>
#include <QFileInfo>
#include <iostream>
#include <QProcessEnvironment>
#include <QDir>


namespace pythonapi {

    static QIssueLogger* logger;
    static QMetaObject::Connection connection;

    bool _initIlwisObjects(){
        int argc = 0;
        char* argv[0];
        QCoreApplication app(argc, argv);
        bool ret = Ilwis::initIlwis(Ilwis::rmCOMMANDLINE);
        pythonapi::logger = new pythonapi::QIssueLogger();
        pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),SIGNAL(ilwiserrormessage(QString)),pythonapi::logger,SLOT(ilwiserrormessage(QString)));
        return ret;
    }

    void disconnectIssueLogger(){
        QObject::disconnect(pythonapi::connection);
    }

    void connectIssueLogger(){
        pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),SIGNAL(ilwiserrormessage(QString)),pythonapi::logger,SLOT(ilwiserrormessage(QString)));
    }
}
