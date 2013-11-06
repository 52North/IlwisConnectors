#ifndef PYTHONAPI_ISSUELOGGER_H
#define PYTHONAPI_ISSUELOGGER_H

#include <QObject>

namespace pythonapi{

    class QIssueLogger: public QObject{
        Q_OBJECT

    public:
        QIssueLogger();

    public slots:
        void ilwiserrormessage(const QString& message);
    };

}

#endif // PYTHONAPI_ISSUELOGGER_H
