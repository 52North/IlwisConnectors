#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include "kernel.h"
#include "ilwisdata.h"
#include "errorobject.h"
#include "numericrange.h"
#include "domain.h"
#include "numericdomain.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "ilwis3connector.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "juliantime.h"
#include "dataformat.h"


using namespace Ilwis;
using namespace Ilwis3;

Ilwis3Connector::Ilwis3Connector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
    QUrl fullname = resolve(resource);
    IniFile *odf = new IniFile();
    odf->setIniFile(fullname.toLocalFile(),load);
    _odf.reset(odf);
    _resource = Resource(fullname, resource.ilwisType());
    if (!load && resource.id() != i64UNDEF)
        _resource.setId(resource.id());
}

bool Ilwis3Connector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    QFileInfo inf = _resource.url().toLocalFile();
    if ( inf.exists()) {
        IniFile *ini = new IniFile();
        ini->setIniFile(inf);
        _odf.reset(ini);
        data->name(inf.fileName());
        data->setDescription(_odf->value("Ilwis","Description"));
        bool ok;
        quint32 sec = _odf->value("Ilwis","Time").toULong(&ok);
        if ( ok) {
            data->createTime((time_t)sec);
            data->modifiedTime( tUNDEF);
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

    if(!willStore(obj))
        return false;

    if ( obj->isReadOnly()) {
        kernel()->issues()->log(QString(WARN_HAS_STATUS2).arg(obj->name().arg("read-only")));
        return false;
    }

    _odf.reset(makeIni(_resource, type));

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
    if ( (obj->ilwisType() & itCOORDSYSTEM))
        _odf->setKeyValue("Ilwis","Type", "CoordSystem");
    if ( (obj->ilwisType() & itDOMAIN))
        _odf->setKeyValue("Ilwis","Type", "Domain");

    //TODO: other types
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
    QString fn = noExt(filename).toLower();

    if ( tp & itDOMAIN){
        table = "numericdomain";
        QString realname = Ilwis3Connector::name2Code(fn,"domain");
        if ( realname != sUNDEF)
            fn = realname;
    }
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

QString Ilwis3Connector::name2Code(const QString& nameIn, const QString& type) {
    QString name = nameIn;
    int index = nameIn.indexOf(".");
    if ( index != -1){
        QString ext = name.mid(index + 1);
        if ( ext == "dom" )
            name = name.left(index).toLower();
    }

    QSqlQuery db(kernel()->database());
    QString query = QString("Select code from aliasses where alias='%1' and type='%2' and source='ilwis3'").arg(name, type);
    if ( !db.exec(query)) {
        kernel()->issues()->logSql(db.lastError());
        return sUNDEF;
    }
    if (!db.next()) {
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
        return sUNDEF;
    }

    QString name = db.value(0).toString();
    return name;
}

QString Ilwis3Connector::provider() const
{
    return "ilwis3";
}

QString Ilwis3Connector::unquote(const QString &name) const
{
    if ( name == sUNDEF || name == "")
        return name;
    if ( name[0] != QChar('\''))
        return name;
    QString newname = name;
    return newname.remove("'");

}

bool Ilwis3Connector::store(IlwisObject *obj, const IOOptions &options)
{
    if(!willStore(obj))
        return false;

    bool ok = true;
    int storemode = options.contains("storemode") ? options["storemode"].toInt() : IlwisObject::smMETADATA | IlwisObject::smBINARYDATA;
    if ( storemode & IlwisObject::smMETADATA)
        ok &= storeMetaData(obj);
    if ( ok && storemode & IlwisObject::smBINARYDATA)
        ok &= storeBinaryData(obj);

    return ok;
}

bool Ilwis3Connector::willStore(const Ilwis::IlwisObject *obj) const
{
    if ( !obj->hasChanged()) { // objects that have not changed and that are linked to a still existing source need no save
        QUrl source = obj->source().url();
        QFileInfo info(source.toLocalFile());
        if ( info.exists()){
            return false;
        }
    }
    if ( obj->isReadOnly()) {
        return ERROR2(WARN_HAS_STATUS2,obj->name(),"read-only");
    }
    return true;
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

    //TODO: other types

    return sUNDEF;
}

IlwisTypes Ilwis3Connector::ilwisType(const QString &name) {
    QString filename = name;
    if (name.contains("?") == 0) {
        filename = name.split("?").front();
    }
    if ( filename.indexOf("file://") == 0) {
        filename = QUrl(filename).toLocalFile();
    }
    QFileInfo inf(filename);
//    bool isCatalog =  inf.isDir();
//    if ( isCatalog)
//        return itCATALOG;

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

    return itUNKNOWN;
}

QString Ilwis3Connector::suffix(IlwisTypes type) {
    if ( type == (itRASTER | itCOLLECTION))
        return "mpl";
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
    QString localName = unquote(name);
    if ( localName.toLower() == "none.grf")
        return "code=georef:undetermined";
    if ( localName != sUNDEF) {
        if ( localName.contains(QRegExp("\\\\|/"))) {
            return localName;
        }
        else {
            if ( owner.isValid())  {
                QString loc = "file:///" + owner.container().toLocalFile() + "/" + localName;
                return loc;
            }
            int index = _odf->file().lastIndexOf("/");
            QUrl loc = _odf->file().left(index) + "/" + localName;
            return loc.toString();

        }
    }
    return sUNDEF;
}

IniFile *Ilwis3Connector::makeIni(const Resource &resource, IlwisTypes type)
{
    QString name = resource.url().toLocalFile();
    QString ext = suffix(type);
    int index = name.lastIndexOf(".");
    if ( index != -1 && index > name.size() - 5){
        name = name.left(index);
    }
    name += "." + ext;
    QFileInfo inf(name);
    IniFile *ini = new IniFile();
    ini->setIniFile(inf, false);

    return ini;
}

QUrl Ilwis3Connector::makeUrl(const QString& path, const QString& name, IlwisTypes type) {
    QString file = path;
    if ( file == "")
        file = _resource.url().toString();
    if ( file.indexOf("file:///") != -1){
        file = QUrl(file).toLocalFile();
    }
    //TODO: container pathing here; grf uses local path
    QFileInfo inf(file);
    QString localpath = inf.absolutePath();
    QString filename =  localpath + "/" + (name != sUNDEF ? name : inf.baseName());
    if ( type != itUNKNOWN){
        int index = filename.lastIndexOf(".");
        if ( index != -1)
            filename = filename.left(index)    ;
        filename += "."+ suffix(type);
    }
    return QUrl::fromLocalFile(filename);
}

QString Ilwis3Connector::outputNameFor(const IlwisObject *obj, bool isMulti, IlwisTypes type) {
    QUrl url = obj->source(IlwisObject::cmOUTPUT).url();
    QString outputName = sUNDEF;
    if ( url.isValid() && url.scheme() == "file") {
        QFileInfo inf(url.toLocalFile());
        outputName = inf.absolutePath() + "/"+ inf.baseName();
    } else {
        QString dir = context()->workingCatalog()->source().toLocalFile();
        outputName =  dir + "/" + obj->name();
    }
    if ( isMulti)
        outputName += "_" + Ilwis3Connector::type2humanName(type) ;
    return outputName;
}

QString Ilwis3Connector::type2humanName(IlwisTypes tp) {
    if ( tp == itPOINT)
        return "point";
    if (tp == itLINE)
        return "seg";
    if ( tp == itPOLYGON)
        return "pol";

    return "feature";
}









