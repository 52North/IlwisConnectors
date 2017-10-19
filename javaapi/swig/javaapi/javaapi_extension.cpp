#include "../../IlwisCore/core/kernel.h"

#include "javaapi_extension.h"
#include "javaapi_qissuelogger.h"
#include "javaapi_error.h"

#include <QtCore/QCoreApplication>
#include <QString>
#include <QSettings>
#include <QFileInfo>
#include <iostream>
#include <QProcessEnvironment>
#include <QDir>


namespace javaapi {

    static QIssueLogger* logger;
    static QMetaObject::Connection connection;
    static QCoreApplication* app = 0;

    bool _initIlwisObjects(const char * ilwisDir){
        int argc = 0;
        char* argv[0];
        app = new QCoreApplication(argc, argv);
        bool ret = Ilwis::initIlwis(Ilwis::rmCOMMANDLINE, ilwisDir);
        javaapi::logger = new javaapi::QIssueLogger();
        javaapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),&Ilwis::IssueLogger::updateIssues,javaapi::logger,&QIssueLogger::ilwiserrormessage);
        return ret;
    }

    void _exitIlwisObjects() {
        if (app)
            delete app;
    }

    void disconnectIssueLogger(){
        QObject::disconnect(javaapi::connection);
    }

    void connectIssueLogger(){
        javaapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),&Ilwis::IssueLogger::updateIssues,javaapi::logger,&QIssueLogger::ilwiserrormessage);
    }
}
