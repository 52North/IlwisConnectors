#include "javaapi_qissuelogger.h"

namespace javaapi{

    QIssueLogger::QIssueLogger(){
    }
    //QT SLOT
    void QIssueLogger::ilwiserrormessage(const Ilwis::IssueObject &issueObject){
        //log(issueObject.message().toStdString());
        // TODO error handling
    }
}
