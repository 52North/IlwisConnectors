#ifndef PYTHONAPI_ISSUELOGGER_H
#define PYTHONAPI_ISSUELOGGER_H

#include <QtCore>

namespace pythonapi{

    class IssueLogger: public QObject{
        Q_OBJECT

    public:
        IssueLogger();

    public slots:
        void ilwiserrormessage(const QString& message);
    };

}

#endif // PYTHONAPI_ISSUELOGGER_H
