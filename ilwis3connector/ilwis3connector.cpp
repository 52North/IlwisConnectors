#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include "inifile.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "errorobject.h"
#include "numericrange.h"
#include "domain.h"
#include "numericdomain.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "juliantime.h"


using namespace Ilwis;
using namespace Ilwis3;


Ilwis3Connector::Ilwis3Connector(const Resource &resource, bool load) : IlwisObjectConnector(resource, load)
{
    QUrl fullname = resolve(resource);
    IniFile *odf = new IniFile();
    odf->setIniFile(fullname, load);
    _odf.reset(odf);
    _resource = Resource(fullname, resource.ilwisType());
    if (!load && resource.id() != i64UNDEF)
        _resource.setId(resource.id());
}

bool Ilwis3Connector::loadMetaData(IlwisObject *data)
{
    QFileInfo inf = _resource.url().toLocalFile();
    if ( inf.exists()) {
        IniFile *ini = new IniFile();
        ini->setIniFile(inf.absoluteFilePath());
        _odf.reset(ini);
        data->setName(inf.fileName());
        data->setDescription(_odf->value("Ilwis","Description"));
        bool ok;
        quint32 sec = _odf->value("Ilwis","Time").toULong(&ok);
        if ( ok) {
            data->setCreateTime((time_t)sec);
            data->setModifiedTime( tUNDEF);
        }
        return true;
    } else {
        kernel()->issues()->log(TR("ODF doesnt exist at %1").arg(inf.absoluteFilePath()));
    }
    return false;
}

bool Ilwis3Connector::storeMetaData(const IlwisObject *obj, IlwisTypes type) const
{
    if ( obj == nullptr)
        return ERROR1(ERR_NO_INITIALIZED_1,"Object");
    if ( obj->isReadOnly()) {
        kernel()->issues()->log(QString(WARN_HAS_STATUS2).arg(obj->name().arg("read-only")));
        return false;
    }

    QString name = _resource.url().toLocalFile();
    QString ext = suffix(type);
    if ( name.indexOf("." + ext) == -1)
        name += "." + ext;
    QFileInfo inf(name);
    IniFile *ini = new IniFile();
    ini->setIniFile(inf.absoluteFilePath(), false);
    _odf.reset(ini);

    _odf->setKeyValue("Ilwis","Description", obj->description());
    _odf->setKeyValue("Ilwis","Time", obj->createTime().toString());
    _odf->setKeyValue("Ilwis","Version", "3.1");
    _odf->setKeyValue("Ilwis","Class", ilwis3ClassName(obj->ilwisType()));
    if ( obj->ilwisType() & itCOVERAGE)
        _odf->setKeyValue("Ilwis","Type", "BaseMap");
    if ( (obj->ilwisType() & itTABLE))
        _odf->setKeyValue("Ilwis","Type", "Table");
    if ( (obj->ilwisType() & itGEOREF))
        _odf->setKeyValue("Ilwis","Type", "GeoRef");

    //TODO other types
    return true;
}

QString Ilwis3Connector::datum2Code(const QString &name, const QString &area) const
{
    QString id = name;
    if ( area != "")
        id += "." + area;
    QSqlQuery db(kernel()->database());
    QString query = QString("Select code from aliasses where alias='%1' and type='datum' and source='ilwis3'").arg(id);
    if ( db.exec(query)) {
        if ( db.next())
            return db.value(0).toString();
    }
    else {
        kernel()->issues()->logSql(db.lastError());
    }

    return sUNDEF;
}

QString Ilwis3Connector::noExt(const QString &name)
{
    int index;
    if ( (index = name.indexOf('.')) > 0)
        return name.left(index);
    return name;
}

bool Ilwis3Connector::isSystemObject(const QString& filename) {
    IlwisTypes tp = ilwisType(filename);
    QString table;


    if ( tp & itDOMAIN)
        table = "numericdomain";
    else if ( tp & itGEODETICDATUM)
        table = "datum";
    else if ( tp & itPROJECTION)
        table = "projection";
    else if ( tp & itELLIPSOID)
        table = "ellipsoide";
    else if ( tp & itCOORDSYSTEM)
        table = "coordinatesystem";

    if ( table.size() == 0) {
        kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(filename));
        return false;
    }

    QString fn = noExt(filename).toLower();

    QSqlQuery db(kernel()->database());
    QString query = QString("Select code from %1 where code='%2'").arg(table, fn);
    if ( !db.exec(query)) {
        kernel()->issues()->logSql(db.lastError());
        return false;
    }
    if (!db.next()) {
        return false;
    }
    return true;
}

QString Ilwis3Connector::name2Code(const QString& name, const QString& type) {
    QSqlQuery db(kernel()->database());
    QString query = QString("Select code from aliasses where alias='%1' and type='%2' and source='ilwis3'").arg(name, type);
    if ( !db.exec(query)) {
        kernel()->issues()->logSql(db.lastError());
        return sUNDEF;
    }
    if (!db.next()) {
        kernel()->issues()->log(TR("Couldnt find %2 %1").arg(type).arg(name));
        return sUNDEF;
    }

    QString code = db.value(0).toString();
    return code;
}

QString Ilwis3Connector::code2name(const QString& code, const QString& type) {
    QSqlQuery db(kernel()->database());
    QString query = QString("Select alias from aliasses where code='%1' and type='%2' and source='ilwis3'").arg(code, type);
    if ( !db.exec(query)) {
        kernel()->issues()->logSql(db.lastError());
        return sUNDEF;
    }
    if (!db.next()) {
        kernel()->issues()->log(TR("Couldnt find %2 %1").arg(type).arg(code));
        return sUNDEF;
    }

    QString name = db.value(0).toString();
    return name;
}

QString Ilwis3Connector::provider() const
{
    return "ilwis3";
}

bool Ilwis3Connector::store(IlwisObject *obj, int storemode)
{
    bool ok = true;
    if ( storemode & IlwisObject::smMETADATA)
        ok &= storeMetaData(obj);
    if ( storemode & IlwisObject::smBINARYDATA)
        ok &= storeBinaryData(obj);

    return ok;
}

QString Ilwis3Connector::ilwis3ClassName(IlwisTypes type) const {
    if ( type & itRASTER)
        return "Raster Map";
    else if ( type & itPOINT)
        return "Point Map";
    else if ( type & itLINE)
        return "Segment Map";
    else if ( type & itPOLYGON)
        return "Polygon Map";
    else if ( type & itTABLE)
        return "Table";

    //TODO other types

    return sUNDEF;
}

IlwisTypes Ilwis3Connector::ilwisType(const QString &name) {
    QString filename = name;
    if (name.contains("?") == 0) {
        filename = name.split("?").front();
    }
    QFileInfo inf(filename);
    bool isCatalog =  inf.isDir();
    if ( isCatalog)
        return itCATALOG;

    QString ext = inf.suffix();
    if ( ext == "mpr")
        return itRASTER;
    if ( ext == "mpa")
        return itPOLYGON;
    if ( ext == "mps")
        return itLINE;
    if ( ext == "mpp")
        return itPOINT;
    if ( ext == "dom")
        return itDOMAIN;
    if ( ext == "csy")
        return itCOORDSYSTEM;
    if ( ext == "grf")
        return itGEOREF;
    if ( ext == "tbt")
        return itTABLE;
    if ( ext == "mpl")
        return itRASTER;
    if ( ext == "ioc")
        return itOBJECTCOLLECTION;

    return itUNKNOWN;
}

QString Ilwis3Connector::suffix(IlwisTypes type) const {
    if ( type == itRASTER)
        return "mpr";
    if ( type == itPOLYGON)
        return "mpa";
    if ( type == itPOINT)
        return "mpp";
    if ( type == itLINE)
        return "mps";
    if ( (type & itDOMAIN) != 0)
        return "dom";
    if ( (type & itGEOREF) != 0)
        return "grf";
    if ( (type & itCOORDSYSTEM) != 0)
        return "csy";
    if ( type == itTABLE)
        return "tbt";
    return  "";
}

QUrl Ilwis3Connector::resolve(const Resource& resource) const {
    QString filename = resource.url().toLocalFile();
    if ( filename == "") {
        if ( resource.name() == sUNDEF)
            return QUrl();

        QUrl url(Ilwis::context()->workingCatalog()->filesystemLocation().toString() + "/" + resource.name());
        filename = url.toLocalFile();
    }
    QFileInfo inf(filename);
    if ( inf.suffix() != ""){
       return QUrl::fromLocalFile(filename);
    }

    IlwisTypes tp = resource.ilwisType();
    bool ok;
    int dim = resource["dimensions"].toInt(&ok);
    QString ext =  ok && tp == itRASTER && dim == 3 ? "mpl" : suffix(tp);
    QString basename = inf.baseName();
    if ( basename == ""){
       basename = resource.name();
       QFileInfo inf(basename);
       if ( inf.suffix() == ext)
           basename = inf.baseName();
    }
    QString path = inf.absolutePath();
    if ( path == "")
        path = Ilwis::context()->workingCatalog()->filesystemLocation().toString();
    else
        path = QUrl::fromLocalFile(path).toString();
    QString file =  basename;
    if ( ext != "")
         file += "." + ext;
    file = path + "/" + file;
    return QUrl(file);
}

QString Ilwis3Connector::filename2FullPath(const QString& name, const Resource& owner) const {
    if ( name != sUNDEF) {
        if ( name.contains(QRegExp("\\\\|/"))) {
            return name;
        }
        else {
            if ( owner.isValid())  {
                QString loc = "file:///" + owner.container().toLocalFile() + "/" + name;
                return loc;
            }
            QUrl loc = context()->workingCatalog()->filesystemLocation();
            return loc.toLocalFile() + "/" + name;

        }
    }
    return sUNDEF;
}








