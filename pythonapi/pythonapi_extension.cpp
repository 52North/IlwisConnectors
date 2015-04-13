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
    static QCoreApplication* app;

    bool _initIlwisObjects(){
        int argc = 0;
        char* argv[0];
        app = new QCoreApplication(argc, argv);
        bool ret = Ilwis::initIlwis(Ilwis::rmCOMMANDLINE);
        pythonapi::logger = new pythonapi::QIssueLogger();
        pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),&Ilwis::IssueLogger::updateIssues,pythonapi::logger,&QIssueLogger::ilwiserrormessage);
        return ret;
    }

    void _exitIlwisObjects() {
        delete app;
    }

    void disconnectIssueLogger(){
        QObject::disconnect(pythonapi::connection);
    }

    void connectIssueLogger(){
        pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),&Ilwis::IssueLogger::updateIssues,pythonapi::logger,&QIssueLogger::ilwiserrormessage);
    }
}
