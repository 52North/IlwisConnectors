#ifndef GATHERERREQUESTHANDLER_H
#define GATHERERREQUESTHANDLER_H

#include "httpserver/httpserver/httprequest.h"
#include "httpserver/httpserver/httpresponse.h"
#include "httpserver/httpserver/httprequesthandler.h"

class QSqlQuery;
class QSqlDatabase;

namespace Ilwis {
namespace Gatherer {


class GathererRequestHandler : public HttpRequestHandler
{
    Q_OBJECT
public:
    GathererRequestHandler();

    void service(HttpRequest& request, HttpResponse& response);
    static HttpRequestHandler *create();

    QString buildQuery(const QString fields, const QString& field, const HttpRequest &request);
private:
    QString packResults(QSqlQuery& db) const;
    void doSelection(const HttpRequest &request, const QString &field, HttpResponse &response);
    void selectTemplate(const QString &templateName, HttpResponse &response);
    void doCreateRequestMap(HttpRequest &request, QString classificationField);
    bool doQuery(const QString &query, QSqlDatabase &db, QSqlQuery &results);
    void createRequestMap(QSqlQuery &results);
    void doTemplateList(HttpResponse &response);
    void modifyDatabase(const HttpRequest &request, HttpResponse &response);
    bool openDatabase(QSqlDatabase &db);
    void writeMessage(const QString &key, const QString &message, HttpResponse &response);
};
}
}

#endif // GATHERERREQUESTHANDLER_H
