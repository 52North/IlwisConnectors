#include <sstream>

#include <QIODevice>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QXmlStreamReader>

#include "kernel.h"
#include "wfsresponse.h"
#include "xpathparser.h"
#include "xmlstreamparser.h"

using namespace Ilwis;
using namespace Wfs;

WfsResponse::WfsResponse()
{
    _networkManager = new QNetworkAccessManager(this);
}

WfsResponse::~WfsResponse()
{
    _iodevice->deleteLater();
    _networkManager->deleteLater();
}

bool WfsResponse::hasFinished() const
{
    return _finished;
}

bool WfsResponse::isException() const
{
    if (!hasFinished()) {
        return false;
    }


    XmlStreamParser parser(device());
    parser.addNamespaceMapping("ows", "http://www.opengis.net/ows");

    return parser.startParsing("ows:ExceptionReport");

}

QString WfsResponse::parseException() const
{
    QString exceptionLog;
    if (!isException()) {
        return exceptionLog;
    }

    XmlStreamParser parser(device());
    parser.addNamespaceMapping("ows", "http://www.opengis.net/ows");

    QXmlStreamReader *reader = parser.reader();
    if (parser.startParsing("ows:ExceptionReport")) {
        QString current = reader->name().toString();
        if (parser.moveTo("ows:Exception"), false) {
            QXmlStreamAttributes attributes = parser.attributes();

        }
    }

    if (reader->hasError()) {
        QString error(reader->error());
        QString message(reader->errorString());
        ERROR2(TR(QString("Unable to parse exception.")).append(" (%1): %2"), error, message);
    }



//    pugi::xml_document doc;
//    std::istringstream cs(_content.toStdString());
//    doc.load(cs);

//    pugi::xpath_node report = doc.select_single_node("/*[local-name()='ExceptionReport']");
//    if (!report) {
//        return false;
//    }

//    pugi::xpath_node_set exceptions = doc.select_nodes("/*/*[local-name()='Exception']");
//    std::for_each(exceptions.begin(), exceptions.end(), [&](pugi::xpath_node exception) {
//        QString exceptionCode = exception.node().attribute("exceptionCode").as_string();
//        QString locator = exception.node().attribute("locator").as_string();

//        QString serverMsg;
//        serverMsg.append(exceptionCode).append(": ");
//        serverMsg.append("'").append(locator).append("' ");

//        QString details("(");
//        pugi::xpath_node_set exceptions = exception.node().select_nodes("*[local-name()='ExceptionText']");
//        std::for_each(exceptions.begin(), exceptions.end(), [&](pugi::xpath_node exception) {
//            details.append(exception.node().text().as_string());
//            details.append(", ");
//        });

//        details.append(").");
//        serverMsg.append(details);
//        exceptionLog.append(serverMsg).append("\n");
//    });


    return exceptionLog;
}


QNetworkReply *WfsResponse::performRequest(QNetworkRequest &request, bool async)
{
    if (async) {
        QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(readResponse(QNetworkReply*)));
    }
    return _networkManager->get(request);
}

//QString WfsResponse::getContent() const
//{
//    QString empty;
//    return _content.isNull() ? empty : _content;
//}

//void WfsResponse::setContent(QString content)
//{
//    if (content.isNull()) {
//        content = "";
//    }
//    _finished = true;
//    _content = content;
//}

QIODevice *WfsResponse::device() const
{
    return _iodevice;
}


void WfsResponse::setDevice(QIODevice *device)
{
    _iodevice = device;
}

void WfsResponse::readResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        //setContent(reply->readAll());
        //_reader->addData(reply->readAll());
        _iodevice = reply;
    } else {
        QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute);
        kernel()->issues()->log(TR("Error: %1)").arg(statusCode.toString()));
        kernel()->issues()->log(TR("Reason: %1)").arg(reason.toString()));
    }


}
