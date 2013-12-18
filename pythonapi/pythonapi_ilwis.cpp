#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/errorobject.h"

#include "pythonapi_ilwis.h"
#include "pythonapi_qissuelogger.h"

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

size_t pythonapi::ilwisErrorObject_type_info(){
    return typeid(Ilwis::ErrorObject).hash_code();
}

const char* pythonapi::get_err_message(std::exception &e){
    Ilwis::ErrorObject* err = dynamic_cast<Ilwis::ErrorObject*>(&e);
    if (err)
        return QString("ILWIS %1: %2").arg(err->what(),err->message()).toLocal8Bit();
    else
        return e.what();
}
