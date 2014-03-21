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
}

bool pythonapi::initIlwisObjects(){
    int argc = 0;
    char* argv[0];
    QCoreApplication app(argc, argv);
    QFileInfo config("./ilwisobjects.conf");
    bool ret = false;
    if (config.exists()){
        ret = Ilwis::initIlwis(config.absoluteFilePath());
    }else{
        QString local = config.absoluteFilePath();
        config.setFile(QDir(QCoreApplication::applicationDirPath()),QString("ilwisobjects.conf"));
        if (config.exists()){
            ret = Ilwis::initIlwis(config.absoluteFilePath());
        }else{
            throw ImportError(QString("None of the following configuration files exist: \n%1\n%2\n runtime directory for ILWIS is unknown!").arg(local,config.absoluteFilePath()).toStdString());
        }
    }
    pythonapi::logger = new pythonapi::QIssueLogger();
    pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),SIGNAL(ilwiserrormessage(QString)),pythonapi::logger,SLOT(ilwiserrormessage(QString)));
    return ret;
}

void pythonapi::disconnectIssueLogger(){
    QObject::disconnect(pythonapi::connection);
}

void pythonapi::connectIssueLogger(){
    pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),SIGNAL(ilwiserrormessage(QString)),pythonapi::logger,SLOT(ilwiserrormessage(QString)));
}
