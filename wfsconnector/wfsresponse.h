#ifndef WFSRESPONSE_H
#define WFSRESPONSE_H

#include "wfsConnector_global.h"

#include <QObject>

class QVariant;
class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;

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
    bool hasFinished();

    /**
     * @brief isException indicates if the service response is an exception.
     * @return true if request failed due to a service exception.
     */
    bool isException();

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
     * @return the response as string.
     */
    QString getContent();

public slots:
    /**
     * @brief readResponse reads the network reply and indicates that hasFinished == true.
     * @param reply the network reply created after a request has been sent.
     */
    void readResponse(QNetworkReply *reply);

private:
    QNetworkAccessManager* _networkManager;
    QString _content;
    bool _finished;
};

#endif // WFSRESPONSE_H
