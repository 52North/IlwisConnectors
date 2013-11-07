#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/errorobject.h"

#include "ilwis.h"
#include "qissuelogger.h"

#include <QtCore/QCoreApplication>
#include <QString>
#include <QFileInfo>

static pythonapi::QIssueLogger* logger;

bool pythonapi::initIlwisObjects(){
    int argc = 0;
    char* argv[0];
    QCoreApplication app(argc, argv);
    bool ret = Ilwis::initIlwis(QFileInfo("C:/Python33/Lib/site-packages/ilwisobjects.conf"));
    logger = new pythonapi::QIssueLogger();
    QObject::connect(Ilwis::kernel()->issues().data(),SIGNAL(ilwiserrormessage(QString)),logger,SLOT(ilwiserrormessage(QString)));
    return ret;
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
