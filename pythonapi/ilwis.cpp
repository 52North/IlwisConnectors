#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/errorobject.h"

#include "ilwis.h"
#include "qissuelogger.h"

#include <QtCore/QCoreApplication>
#include <QString>
#include <QFileInfo>

namespace pythonapi {

    static QIssueLogger* logger;
    static QMetaObject::Connection connection;

}

bool pythonapi::initIlwisObjects(){
    int argc = 0;
    char* argv[0];
    QCoreApplication app(argc, argv);
    bool ret = Ilwis::initIlwis(QFileInfo("C:/Python33/Lib/site-packages/ilwisobjects.conf"));
    pythonapi::logger = new pythonapi::QIssueLogger();
    pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),SIGNAL(ilwiserrormessage(QString)),pythonapi::logger,SLOT(ilwiserrormessage(QString)));
    return ret;
}

void pythonapi::muteIssueLogger(){
    QObject::disconnect(pythonapi::connection);
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
