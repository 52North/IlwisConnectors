#include "../../IlwisCore/core/kernel.h"

#include "pythonapi_extension.h"
#include "pythonapi_qissuelogger.h"
#include "pythonapi_error.h"

#include <QtCore/QCoreApplication>
#include <QString>
#include <QSettings>
#include <QFileInfo>
#include <iostream>
#include <QProcessEnvironment>
#include <QDir>


namespace pythonapi {

    static QIssueLogger* logger;
    static QMetaObject::Connection connection;
    static QCoreApplication* app = 0;

    bool _initIlwisObjects(const char * ilwisDir){
        int argc = 0;
        char* argv[0];
        bool ret = true;

        app = QCoreApplication::instance();
        if (!app){
            app = new QCoreApplication(argc, argv);
            ret = Ilwis::initIlwis(Ilwis::rmCOMMANDLINE, ilwisDir);
        }
        pythonapi::logger = new pythonapi::QIssueLogger();
        pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),&Ilwis::IssueLogger::updateIssues,pythonapi::logger,&QIssueLogger::ilwiserrormessage);
        return ret;
    }

    void _exitIlwisObjects() {
        if (app)
            delete app;
    }

    void disconnectIssueLogger(){
        QObject::disconnect(pythonapi::connection);
    }

    void connectIssueLogger(){
        pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),&Ilwis::IssueLogger::updateIssues,pythonapi::logger,&QIssueLogger::ilwiserrormessage);
    }
}
