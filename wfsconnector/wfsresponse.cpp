#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include "pugixml/pugixml.hpp"

#include "kernel.h"
#include "ilwisdata.h"
#include "wfsresponse.h"

WfsResponse::WfsResponse()
{
    _networkManager = new QNetworkAccessManager(this);
}

WfsResponse::~WfsResponse()
{
    _networkManager->deleteLater();
}

bool WfsResponse::hasFinished()
{

}

bool WfsResponse::isException()
{

}

QNetworkReply *WfsResponse::performRequest(QNetworkRequest &request, bool async)
{
    if (async) {
        QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(readResponse(QNetworkReply*)));
    }
    return _networkManager->get(request);
}

QString WfsResponse::getContent()
{
    QString empty;
    return _content == "" ? empty : _content;
}

void WfsResponse::readResponse(QNetworkReply *reply)
{
    //_statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (reply->error() == QNetworkReply::NoError)
    {
        _content = reply->readAll();
    }

    // set state and cleanup after response is complete
    _finished = true;
    reply->deleteLater();
}
