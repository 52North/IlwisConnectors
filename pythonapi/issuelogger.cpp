#include "issuelogger.h"

#include "log.h"

using namespace pythonapi;

IssueLogger::IssueLogger(){
}

void IssueLogger::ilwiserrormessage(const QString &message){
    pythonapi::log(message.toLocal8Bit());
}
