#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QHash>
#include <fstream>

#include "kernel.h"
#include "ilwiscontext.h"
#include "numericrange.h"
#include "ilwisdata.h"
#include "raster.h"
#include "inifile.h"
#include "numericdomain.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "ilwis3objectfactory.h"
#include "domainconnector.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "columndefinition.h"
#include "table.h"
#include "containerstatistics.h"
#include "pixeliterator.h"
#include "conventionalcoordinatesystem.h"
#include "coordinatesystemconnector.h"
#include "ellipsoidconnector.h"
#include "ilwis3projectionconnector.h"
#include "georefconnector.h"
#include "rawconverter.h"
#include "coverageconnector.h"
#include "gridcoverageconnector.h"
#include "tableconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

Ilwis3ObjectFactory::Ilwis3ObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","ilwis3","")
{
}

IlwisObject *Ilwis3ObjectFactory::create(const Resource &item) const
{

    //Ilwis3Connector *connector = ConnectorFactory::screate<Ilwis3Connector>(item, "ilwis3");

     const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
     Ilwis3Connector *connector = factory->createFromResource<Ilwis3Connector>(item, "ilwis3");

    if(!connector) {
        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",item.name()));
        return 0;
    }
    IlwisObject *object = createObject(connector);
    if ( object)
        return object;

    delete connector;
    return 0;
}

bool Ilwis3ObjectFactory::canUse(const Resource &item) const
{
    if ( item.url().scheme() == "ilwis") // can't use anything marked as internal
        return false;

    if ( item.url().scheme() != "file") // can't read non file based data
        return false;


    IlwisTypes type = item.ilwisType() ;
    QHash<IlwisTypes, CheckUsage>::const_iterator cur;
    if ( (cur = _types.find(type)) != _types.end()) {
        return (cur.value())(item);
    }
    QString filename = item.url().toLocalFile();
    if ( filename == "")
        return false;

    QString ext = QFileInfo(filename).suffix();
    QString exts = "mprmpamppmpsdomtbtgrfcsympl";
    if ( exts.indexOf(ext) == -1)
        return false;

    if ( type & itDOMAIN)
        return true;
    else if ( type & itGEODETICDATUM)
        return true;
    else if ( type & itELLIPSOID)
        return true;
    else if ( type & itPROJECTION)
        return true;
    else if ( type & itCOORDSYSTEM)
        return true;
    else if ( type & itGEOREF)
        return true;
    else if ( type & itGRID)
        return true;
    else if ( type & itTABLE)
        return true;
    else if ( type & itPOINT)
        return true;
    else if ( type & itLINE)
        return true;
    else if ( type & itPOLYGON)
        return true;

    return false;
}

bool Ilwis3ObjectFactory::prepare()
{
    loadIlwis3Aliasses();
    _types[itCOORDSYSTEM] = (CheckUsage) Ilwis3::CoordinateSystemConnector::canUse;

    return true;
}

void Ilwis3ObjectFactory::loadIlwis3Aliasses() {
    QSqlQuery sqlPublic(kernel()->database());
    insertFile("datum_allias.csv", sqlPublic);
    insertFile("ellipsoid_allias.csv",sqlPublic);
    insertFile("projection_allias.csv",sqlPublic);
    insertFile("domain_allias.csv",sqlPublic);
}

void Ilwis3ObjectFactory::insertFile(const QString& filename, QSqlQuery& sqlPublic) {
    QString basePath = context()->ilwisFolder().absoluteFilePath() + "/extensions/ilwis3connector/resources";
    QFileInfo fileInfo(basePath + "/" + filename);
    if ( !fileInfo.exists()) {
        kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(filename), IssueObject::itCritical);
        return;
    }

    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(filename), IssueObject::itCritical);
        return;
    }
    QTextStream txtFile(&file);
    bool skip = true;
    while(!txtFile.atEnd()) {
        QString line = txtFile.readLine();
        if ( skip) { // first line is header
            skip = false;
            continue;
        }
        QStringList parts = line.split(',');


        bool ok = false;
        if ( filename == "datum_allias.csv")
            ok = fillDatumRecord(parts, sqlPublic);
        else if ( filename == "ellipsoid_allias.csv")
            ok = fillEllipsoidRecord(parts, sqlPublic);
        else if ( filename == "projection_allias.csv")
            ok = fillProjectionRecord(parts, sqlPublic);
        else if ( filename == "domain_allias.csv")
            ok = fillDomainRecord(parts, sqlPublic);

        if (!ok) {
            kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(filename), IssueObject::itCritical);
            return;
        }

    }
}

bool Ilwis3ObjectFactory::fillDomainRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 1 && parts[0] == "") // empty line
        return true;
    if ( parts.size() != 2 || parts[0] == "" )
        return false;
    if ( parts[1] == "") // not necessarily corrupt, just an unfinished record
        return true;
   QString parms = QString("'%1','%2','domain','ilwis3'").arg(parts[1],parts[0]);
   QString stmt = QString("INSERT INTO aliasses VALUES(%1)").arg(parms);

   return doQuery(stmt, sqlPublic);
}

bool Ilwis3ObjectFactory::fillEllipsoidRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 1 && parts[0] == "") // empty line
        return true;
    if ( parts.size() != 2 || parts[0] == "" )
        return false;
    if ( parts[1] == "") // not necessarily corrupt, just an unfinished record
        return true;
   QString parms = QString("'%1','%2','ellipsoid','ilwis3'").arg(parts[0],parts[1]);
   QString stmt = QString("INSERT INTO aliasses VALUES(%1)").arg(parms);

   return doQuery(stmt, sqlPublic);
}

bool Ilwis3ObjectFactory::fillProjectionRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 1 && parts[0] == "") // empty line
        return true;
    if ( parts.size() != 2 || parts[0] == "" )
        return false;
    if ( parts[1] == "") // not necessarily corrupt, just an unfinished record
        return true;
   QString parms = QString("'%1','%2','projection','ilwis3'").arg(parts[0],parts[1]);
   QString stmt = QString("INSERT INTO aliasses VALUES(%1)").arg(parms);

   return doQuery(stmt, sqlPublic);
}

bool Ilwis3ObjectFactory::fillDatumRecord(const QStringList& parts, QSqlQuery &sqlPublic) {
    if ( parts.size() == 1 && parts[0] == "") // empty line
        return true;
    if ( parts.size() != 3 || parts[0] == "" )
        return false;
    if ( parts[2] == "") // not necessarily corrupt, just an unfinished record
        return true;
    QString id = parts[0];
    if ( parts[1] != "")
       id += "." + parts[1];
    QString parms = QString("'%1','%2','datum','ilwis3'").arg(id,parts[2]);
    QString stmt = QString("INSERT INTO aliasses VALUES(%1)").arg(parms);

    return doQuery(stmt, sqlPublic);

}

bool Ilwis3ObjectFactory::doQuery(QString &query, QSqlQuery &sqlPublic)
{
    bool ok = sqlPublic.exec(query);
    if (!ok) {
        kernel()->issues()->logSql(sqlPublic.lastError());
        return false;
    }
    return true;
}
