#include "kernel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QXmlStreamWriter>
#include "geometries.h"
#include "ilwiscontext.h"
#include "ilwisconfiguration.h"
#include "gathererrequesthandler.h"

using namespace Ilwis;
using namespace Gatherer;

GathererRequestHandler::GathererRequestHandler()
{
}

void GathererRequestHandler::service(HttpRequest &request, HttpResponse &response)
{
    QString encoding = ilwisconfig("server-settings/encoding",QString("UTF-8"));
    response.setHeader("Content-Type", qPrintable("text/plain; charset="+encoding));
    QString templateName = request.getParameter("template");
    if ( templateName != "" ){
        selectTemplate(templateName, response);
    } else {
        QString selectField = request.getParameter("selectfield");
        if ( selectField != "") {
            doSelection(request, selectField, response);
        }
    }
}

void GathererRequestHandler::selectTemplate(const QString& templateName, HttpResponse &response){
    QString docroot = ilwisconfig("server-settings/template-root-path", QString("."));
    QString resource = docroot + "/" + templateName + ".template";

    QFile file(resource);
    if (file.open(QIODevice::ReadOnly)) {
        while (!file.atEnd() && !file.error()) {
                response.write(file.read(65536));
        }
        file.close();
    }
    return;
}

void GathererRequestHandler::doSelection(const HttpRequest &request, const QString& selectField, HttpResponse &response){
    QString beginTime = request.getParameter("begintime");
    QString endTime = request.getParameter("endtime");
    QString envelope = request.getParameter("envelope");
    QStringList coords = envelope.split(",");
    LatLon c1(coords[0], coords[1]);
    LatLon c2(coords[2], coords[3]);
    std::vector<QString> queryParts;
    QString query = "select observationid,observation,observationtype,observationtime,observer,ST_AsText(location) from \"FieldData\" where";
    query += " template = '" + selectField + "'";
    if ( beginTime != "" && endTime != "")
        queryParts.push_back(" observationtime >= '" + beginTime + "' and observationtime <= '" + endTime + "'");
    else if (beginTime != ""){
        queryParts.push_back(" observationtime >= '" + beginTime + "'");
    }else if (endTime != "" )
        queryParts.push_back(" observation <= '" + endTime + "'");

    if ( c1.isValid() && c2.isValid()){
        Envelope env(c1, c2);
        QString envs = QString("%1,%2,%3,%4").arg(env.min_corner().y).arg(env.min_corner().x).arg(env.max_corner().y).arg(env.max_corner().x);
        queryParts.push_back("location && ST_MakeEnvelope("+ envs + ")");
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL","GathererConnection");
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("Gatherer");
    db.setUserName("postgres");
    db.setPassword("visconti");
    db.setPort(5432);
    bool ok = db.open();
    if(ok != true)
    {
        qDebug("Connection Failed!");
    }
    else
    {
        QString selection;
        for(auto part : queryParts){
            if ( selection.size() != 0)
                selection += " and ";
            selection += part;
        }
        if ( selection != "")
            query += " and " + selection;
        auto results = db.exec(query);
        QString xml = packResults(results);
        response.write(xml.toLocal8Bit());
    }
    db.close();


}

QString GathererRequestHandler::packResults(QSqlQuery& results) const{
    QString output;
    QXmlStreamWriter writer(&output);
    writer.writeStartDocument("1.0",true);
    writer.writeStartElement("GathererObservations");
    writer.writeAttribute("version", "1.0");

    while ( results.next()) {
        QSqlRecord rec = results.record();
        QString observationid = rec.field("observationid").value().toString();
        QString observation = rec.field("observation").value().toString();
        int observationtype = rec.field("observationtype").value().toInt();
        QString timestamp = rec.field("observationtime").value().toString();
        QString observer = rec.field("observer").value().toString();
        QString location = rec.field("location").value().toString();
        writer.writeStartElement("Observation");
        writer.writeAttribute("observationid",observationid);
        writer.writeTextElement("Observation", observation);
        writer.writeTextElement("ObservationType", QString::number(observationtype));
        writer.writeTextElement("TimeStamp", timestamp);
        writer.writeTextElement("Observer", observer);
        writer.writeTextElement("Location", location);
        writer.writeEndElement();


    }
    writer.writeEndDocument();
    return output;
}

HttpRequestHandler *GathererRequestHandler::create()
{
    return new GathererRequestHandler();
}
