#include "pythonapi_ilwis.h"

#include "pythonapi_qissuelogger.h"

using namespace pythonapi;

QIssueLogger::QIssueLogger(){
}
//QT SLOT
void QIssueLogger::ilwiserrormessage(const QString &message){
    pythonapi::log(message.toStdString());
}
