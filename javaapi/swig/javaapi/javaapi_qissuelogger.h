#ifndef JAVAAPI_ISSUELOGGER_H
#define JAVAAPI_ISSUELOGGER_H

#include "../../IlwisCore/core/issuelogger.h"
#include <QObject>

namespace javaapi{

    class QIssueLogger: public QObject{
        Q_OBJECT

    public:
        QIssueLogger();

    public slots:
        void ilwiserrormessage(const Ilwis::IssueObject& issueObject);
    };

}

#endif // JAVAAPI_ISSUELOGGER_H
