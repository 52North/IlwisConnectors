#include "qissuelogger.h"

#include "ilwis.h"

using namespace pythonapi;

QIssueLogger::QIssueLogger(){
}
//QT SLOT
void QIssueLogger::ilwiserrormessage(const QString &message){
    pythonapi::log(message.toLocal8Bit());
}
