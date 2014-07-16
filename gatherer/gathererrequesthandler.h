#ifndef GATHERERREQUESTHANDLER_H
#define GATHERERREQUESTHANDLER_H

#include "httpserver/httpserver/httprequest.h"
#include "httpserver/httpserver/httpresponse.h"
#include "httpserver/httpserver/httprequesthandler.h"

class QSqlQuery;

namespace Ilwis {
namespace Gatherer {


class GathererRequestHandler : public HttpRequestHandler
{
    Q_OBJECT
public:
    GathererRequestHandler();

    void service(HttpRequest& request, HttpResponse& response);
    static HttpRequestHandler *create();

private:
    QString packResults(QSqlQuery& db) const;
    void doSelection(const HttpRequest &request, const QString &selectField, HttpResponse &response);
    void selectTemplate(const QString &templateName, HttpResponse &response);
};
}
}

#endif // GATHERERREQUESTHANDLER_H
