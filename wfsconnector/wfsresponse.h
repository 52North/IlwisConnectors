#ifndef WFSRESPONSE_H
#define WFSRESPONSE_H

#include <QObject>

#include "wfsConnector_global.h"

class QVariant;
class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsResponse: QObject
{

    Q_OBJECT

public:
    WfsResponse();
    ~WfsResponse();

    /**
     * @brief hasFinished indicates if the request is still running or not (for async requests).
     * @return false if request is still pending and not (completely) received yet.
     */
    bool hasFinished() const;

    /**
     * Checks is service response is an exception message. Please note that for asyncronous
     * requests to check via hasFinished() if the response has already been received.
     *
     * @brief isException indicates if the service response is an exception.
     * @return true if request failed due to a service exception.
     */
    bool isException() const;

    /**
     * Starts performing request by means of the underlying network manager. The request
     * can be triggered synchronously or asynchronously. After the response is finished
     * and has been read it is being deleted.
     *
     * @brief performRequest starts async network communication.
     * @param request the actual request to perform.
     * @param async true if request shall be aysncronous (the default), or false otherwise.
     * @return the response (though, request may still be running).
     */
    QNetworkReply *performRequest(QNetworkRequest &request, bool async=true);

    /**
     * @brief getContent reads the response content.
     * @return the response as string (never null).
     */
    QString getContent() const;

    /**
     * @brief setContent sets the responds content explicitly (useful for tests).
     * @param content the content to set (if null, an empty string is set).
     */
    void setContent(QString content);

public slots:
    /**
     * @brief readResponse reads the network reply and indicates that hasFinished == true.
     * @param reply the network reply created after a request has been sent.
     */
    void readResponse(QNetworkReply *reply);

private:
    QNetworkAccessManager* _networkManager;
    QString _content = "";
    bool _finished = false;
};
}
}

#endif // WFSRESPONSE_H
