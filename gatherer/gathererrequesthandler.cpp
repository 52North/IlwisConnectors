#include "kernel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QDirIterator>
#include <QXmlStreamWriter>
#include "geometries.h"
#include "ilwiscontext.h"
#include "ilwisconfiguration.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/Coordinate.h"
#include "geometryhelper.h"
#include "gathererrequesthandler.h"

using namespace Ilwis;
using namespace Gatherer;

GathererRequestHandler::GathererRequestHandler()
{
}

void GathererRequestHandler::createRequestMap(QSqlQuery& results)
{
    ICoordinateSystem csy("code=epsg:4326");
    std::vector<geos::geom::Geometry *> collection;
    while ( results.next()) {
        QString wkt = results.value("location").toString();
        geos::geom::Geometry *geom = GeometryHelper::fromWKT(wkt,csy.ptr());
        if ( geom)
            collection.push_back(geom);
    }
    //std::unique_ptr<geos::geom::GeometryFactory> factor = std::make_unique( )
   // geos::geom::MultiPoint *multipoint = geos::geom::GeometryFactory
}

void GathererRequestHandler::doCreateRequestMap(HttpRequest &request, QString classificationField)
{
    QString fields = "ST_AsText(location)";
    QString query = buildQuery(fields, classificationField, request);
    QSqlDatabase db;
    QSqlQuery results;
    if (doQuery(query,db, results)) {
        createRequestMap(results);
    }
    db.close();


}

void GathererRequestHandler::doTemplateList(HttpResponse &response){
    QString docroot = ilwisconfig("server-settings/template-root-path", QString("."));
    QStringList filters;
    filters << "*.template";
    QDir folder(docroot);
    QFileInfoList files = folder.entryInfoList(filters);
    QString result;
    for(auto file : files) {
        if ( result != "")
            result += ";";
        result += file.baseName();
    }
    response.write(result.toLocal8Bit());
}

void GathererRequestHandler::writeMessage(const QString& key, const QString& message, HttpResponse &response) {
    QString output;
    QXmlStreamWriter writer(&output);
    writer.writeStartDocument("1.0",true);
    writer.writeStartElement("GathererObservations");
    writer.writeAttribute("version", "1.0");
    writer.writeTextElement(key, message);
    writer.writeEndDocument();
    response.write(output.toLocal8Bit());
}

void GathererRequestHandler::service(HttpRequest &request, HttpResponse &response)
{
    QString encoding = ilwisconfig("server-settings/encoding",QString("UTF-8"));
    response.setHeader("Content-Type", qPrintable("text/plain; charset="+encoding));
    QMultiMap<QByteArray,QByteArray> parameters = request.getParameterMap();
    QMultiMap<QByteArray,QByteArray>::Iterator iter;
    if ( (iter = parameters.find("subject")) != parameters.end() )
        selectTemplate(iter.value(), response);
    else if ( (iter = parameters.find("subjectquery")) != parameters.end())
        doSelection(request, iter.value(), response);
    else if ( (iter = parameters.find("createrequestmap")) != parameters.end())
        doCreateRequestMap(request, iter.value());
    else if ( (iter = parameters.find("subjectlist")) != parameters.end())
        doTemplateList(response);
    else if ( (iter = parameters.find("subjectobservation")) != parameters.end())
        modifyDatabase(request, response);
    else{
        writeMessage("Error", "No valid query in GET request", response);
    }

}

void GathererRequestHandler::modifyDatabase(const HttpRequest &request, HttpResponse &response){
    QString observationid = request.getParameter("observationid");
    QString location = request.getParameter("location");
    QString time = request.getParameter("time");
    QString subject = request.getParameter("subjectname");
    QString observer = request.getParameter("observer");
    QMultiMap<QByteArray,QByteArray> params = request.getParameterMap();
    std::map<QString, QString> fields;
    for(auto iter = params.begin(); iter != params.end(); ++iter)    {
        if ( iter.key().indexOf("field") == 0){
            fields[iter.key()] = iter.value();
        }
    }
    bool insert = observationid == "";
    QSqlDatabase db;
    if (!openDatabase(db)){
        return ;
    }

    for(auto iter = fields.begin(); iter != fields.end(); ++iter){
        if (insert ){
            QString insertStmt = "insert into \"FieldData\" (template, \
                    observation,observationtype,observationtime,location,observer,fieldid) values(";
           insertStmt += "'" + subject + "',";
           insertStmt += "'" + iter->second + "',";
           insertStmt += QString::number(1) + ",";
           insertStmt += "'" + time + "',";
           insertStmt += "ST_GeomFromText('" + location + "',4326),";
           insertStmt += "'" + observer + "',";
           insertStmt += "'" + iter->first + "')";
           QSqlQuery ret = db.exec(insertStmt);
           int id = ret.lastInsertId().toInt();
           QString message = QString("insert succesfull : last inserted id = %1" ).arg(id);
           writeMessage("Result", message, response);
        } else {
            QString updateStmt = "update \"FieldData\" where observationid=" + observationid + "";
            updateStmt += " set location= ST_GeomFromText('" + location + "',4326)";
            updateStmt += ", time='" + time + "'";
            updateStmt += ", template='" + subject + "'";
            updateStmt += ",observer='" + observer + "'";
            updateStmt += ",fieldid='" + iter->first + "'";
            updateStmt += ",observation='" + iter->second + "'";
            db.exec(updateStmt);
        }
    }

    db.close();


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

QString GathererRequestHandler::buildQuery(const QString fields, const QString& classificationField, const HttpRequest &request)
{
    QString beginTime = request.getParameter("begintime");
    QString endTime = request.getParameter("endtime");
    QString envelope = request.getParameter("envelope");
    QStringList coords = envelope.split(",");
    if ( coords.size() != 4)
        return sUNDEF;

    LatLon c1(coords[0], coords[1]);
    LatLon c2(coords[2], coords[3]);
    std::vector<QString> queryParts;
    QString query = "select " + fields + " from \"FieldData\" where";
    query += " template = '" + classificationField + "'";
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
    QString selection;
    for(auto part : queryParts){
        if ( selection.size() != 0)
            selection += " and ";
        selection += part;
    }
    if ( selection != "")
        query += " and " + selection;

    return query;
}

bool GathererRequestHandler::openDatabase(QSqlDatabase& db){
    db = QSqlDatabase::addDatabase("QPSQL","GathererConnection");
    db.setHostName("localhost");
    db.setDatabaseName("Gatherer");
    db.setUserName("postgres");
    db.setPassword("visconti");
    db.setPort(5432);
    bool ok = db.open();
    if(!ok)
    {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"connection", "GathererConnection");
    }
    return true;
}

bool GathererRequestHandler::doQuery(const QString& query, QSqlDatabase& db, QSqlQuery& results){
    if (openDatabase(db))
    {
        results = db.exec(query);
        if ( results.lastError().isValid())
            return false;
        return true;
    }
    return false;
}

void GathererRequestHandler::doSelection(const HttpRequest &request, const QString& classificationField, HttpResponse &response){
    QString fields = "observationid,observation,observationtype,observationtime,observer,fieldid,ST_AsText(location) as location";
    QString query = buildQuery(fields, classificationField, request);
    QSqlDatabase db;
    QSqlQuery results;
    if (doQuery(query,db, results)) {
        QString xml = packResults(results);
        response.write(xml.toLocal8Bit());
    } else {
        writeMessage("Error", "No valid database response", response);
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
        QString fieldid = rec.field("fieldid").value().toString();
        writer.writeStartElement("Observation");
        writer.writeAttribute("observationid",observationid);
        writer.writeTextElement("Observation", observation);
        writer.writeTextElement("ObservationType", QString::number(observationtype));
        writer.writeTextElement("TimeStamp", timestamp);
        writer.writeTextElement("Observer", observer);
        writer.writeTextElement("Location", location);
        writer.writeTextElement("FieldId", fieldid);
        writer.writeEndElement();


    }
    writer.writeEndDocument();
    return output;
}

HttpRequestHandler *GathererRequestHandler::create()
{
    return new GathererRequestHandler();
}
