#include <sstream>

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include "pugixml/pugixml.hpp"

#include "kernel.h"
#include "wfsresponse.h"

using namespace Ilwis;
using namespace Wfs;

WfsResponse::WfsResponse()
{
    _networkManager = new QNetworkAccessManager(this);
}

WfsResponse::~WfsResponse()
{
    _networkManager->deleteLater();
}

bool WfsResponse::hasFinished() const
{
    return _finished;
}

bool WfsResponse::isException() const
{
    if (!hasFinished()) {
        kernel()->issues()->log(TR("Request has not finished yet!"));
        return false;
    }
    if (_content.isEmpty()) {
        kernel()->issues()->log(TR("No content after request has finished!"));
        return false;
    }
    pugi::xml_document doc;
    std::istringstream cs(_content.toStdString());
    doc.load(cs);

    pugi::xpath_node report = doc.select_single_node("/*[local-name()='ExceptionReport']");
    if (!report) {
        return false;
    }

    QString exceptionLog;
    pugi::xpath_node_set exceptions = doc.select_nodes("/*/*[local-name()='Exception']");
    std::for_each(exceptions.begin(), exceptions.end(), [&](pugi::xpath_node exception) {
        QString exceptionCode = exception.node().attribute("exceptionCode").as_string();
        QString locator = exception.node().attribute("locator").as_string();

        QString serverMsg;
        serverMsg.append(exceptionCode).append(": ");
        serverMsg.append("'").append(locator).append("' ");

        QString details("(");
        pugi::xpath_node_set exceptions = exception.node().select_nodes("*[local-name()='ExceptionText']");
        std::for_each(exceptions.begin(), exceptions.end(), [&](pugi::xpath_node exception) {
            details.append(exception.node().text().as_string());
            details.append(", ");
        });

        details.append(").");
        serverMsg.append(details);
        exceptionLog.append(serverMsg).append("\n");
    });

    kernel()->issues()->log(TR("Server responded with an exception: \n%1").arg(exceptionLog));
    return true;
}


QNetworkReply *WfsResponse::performRequest(QNetworkRequest &request, bool async)
{
    if (async) {
        QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(readResponse(QNetworkReply*)));
    }
    return _networkManager->get(request);
}

QString WfsResponse::getContent() const
{
    QString empty;
    return _content.isNull() ? empty : _content;
}

void WfsResponse::setContent(QString content)
{
    if (content.isNull()) {
        content = "";
    }
    _finished = true;
    _content = content;
}

void WfsResponse::readResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        setContent(reply->readAll());
    } else {
        QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute);
        kernel()->issues()->log(TR("Error: %1)").arg(statusCode.toString()));
        kernel()->issues()->log(TR("Reason: %1)").arg(reason.toString()));
    }
    // cleanup network resources
    reply->deleteLater();
}
