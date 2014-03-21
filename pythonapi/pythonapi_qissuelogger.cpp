#include "pythonapi_qissuelogger.h"

#include "pythonapi_pyobject.h"

namespace pythonapi{

    QIssueLogger::QIssueLogger(){
    }
    //QT SLOT
    void QIssueLogger::ilwiserrormessage(const QString &message){
        log(message.toStdString());
    }
}
