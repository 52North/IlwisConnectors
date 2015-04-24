#include <QRegExp>
#include <QFileInfo>
#include <QUrl>
#include <QStringList>
#include <QDir>
#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "geometries.h"
#include "identity.h"
#include "resource.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "odfitem.h"
#include "ilwis3connector.h"

const QString Ilwis::Ilwis3::ODFItem::systemObjectNames="value;image;image;min1to1;nilto1;count;distance;ndvi;percentage;none;latlonwgs84;latlon;bool;byte;color;colorcmp";

using namespace Ilwis;
using namespace Ilwis3;

#include <typeinfo>

ODFItem::ODFItem(const QFileInfo &file) : Resource(QUrl::fromLocalFile(file.absoluteFilePath()),itANY), _projectionName(sUNDEF)
{
    _odf.setIniFile(file);
    _file = file;
    name(_file.fileName(), false);
    addContainer(QUrl::fromLocalFile(_file.canonicalPath()));
    IlwisTypes csytp, domtp, grftp;
    csytp = domtp = grftp = itUNKNOWN;

    setDescription( _odf.value("Ilwis", "Description"));

    _ilwtype = Ilwis3Connector::ilwisType(_file.absoluteFilePath());
    _csyname = findCsyName(file.absoluteFilePath());
    _domname = findDomainName(file.absoluteFilePath());
    _grfname = findGrfName();
    _datumName = findDatumName();


    csytp = findCsyType(file.absoluteFilePath());
    domtp = findDomainType(file.absoluteFilePath());


    if ( csytp != itUNKNOWN && _ilwtype == itCOORDSYSTEM)
        _ilwtype = csytp;
    if ( domtp != itUNKNOWN && _ilwtype == itDOMAIN)
        _ilwtype = domtp;

    if ( _domname != sUNDEF)
        _extendedType |= domtp;

    if ( _csyname != sUNDEF) {
        _extendedType |= csytp;
        _projectionName = findProjectionName();
        if ( _projectionName != sUNDEF)
            _extendedType |= itPROJECTION;
    }
    if ( _grfname != sUNDEF)
        _extendedType |= itGEOREF;


    if (_datumName != sUNDEF)
        _extendedType |= itGEODETICDATUM;

    _size = findSize();
    _dimensions = findDimensions();
}


bool ODFItem::resolveNames(const QHash<QString, quint64> &names)
{
    bool ok = true;
    quint64 fileid = i64UNDEF;
    ok &= setFileId(names, _domname.toLower(), fileid);
    if ( fileid != i64UNDEF)
        _properties["domain"] = fileid;
    ok &= setFileId(names, _grfname.toLower(), fileid);
    if ( fileid != i64UNDEF)
        _properties["georeference"] = fileid;
    ok &= setFileId(names, _csyname.toLower(), fileid);
    if ( fileid != i64UNDEF)
        _properties["coordinatesystem"] = fileid;
    if ( _datumName != sUNDEF) {
        const Resource& resource = mastercatalog()->name2Resource(_datumName,itGEODETICDATUM);
        _properties["geodeticdatum"] = resource.id();
    }
    if (_projectionName != sUNDEF) {
        const Resource& resource = mastercatalog()->name2Resource(_projectionName,itPROJECTION);
        _properties["projection"] = resource.id();
    }

    return ok;
}

bool ODFItem::setFileId(const QHash<QString, quint64> &names, const QString& value, quint64 &fileid) const {
    if ( value == sUNDEF) {
        fileid = i64UNDEF;
        return true; // legal; some properties might not have a value(name)
    }
    if ( Ilwis3Connector::ilwisType(value) & itCOORDSYSTEM) {
        if ( value == "latlonwgs84.csy" ) {
            Resource resource = mastercatalog()->name2Resource("code=epsg:4326", itCOORDSYSTEM);
            if ( !resource.isValid()) {
               return ERROR1(ERR_FIND_SYSTEM_OBJECT_1, "Wgs 84");
            }
            fileid = resource.id();
            return true;
        }
        if ( value == "unknown.csy" ) {
            Resource resource = mastercatalog()->name2Resource("code=csy:unknown", itCOORDSYSTEM);
            if ( !resource.isValid()) {
                return ERROR1(ERR_FIND_SYSTEM_OBJECT_1, "'Unknown' coordinate system");
            }
            fileid = resource.id();
            return true;
        }
    }
    if ( Ilwis3Connector::ilwisType(value) & itGEOREF) {
        if ( value == "none.grf" ) {
            Resource resource = mastercatalog()->name2Resource("code=georef:undetermined", itGEOREF);
            if ( !resource.isValid()) {
                return ERROR1(ERR_FIND_SYSTEM_OBJECT_1, "'undetermined' georeference");
            }
            fileid = resource.id();
            return true;
        }
    }
    QString completeName =  (value.contains(QRegExp("\\\\|/"))) ? value : _file.canonicalPath() + "/" + value;
    QUrl url = QUrl::fromLocalFile(completeName.toLower());
    QString dd = url.toLocalFile();
    QHash<QString, quint64>::const_iterator iter = names.find(url.toString());
    if (iter != names.end()){
        fileid = iter.value();
    } else {
        // at this time we can't rely on the working catalog to be set(if we are initializing it), so no normal resolve
        // the mastercatalog will contain system items at this moment so we can check these first
        QString baseName = value.left(value.indexOf("."));
        IlwisTypes tp = Ilwis3Connector::ilwisType(value);
        Resource resource = mastercatalog()->name2Resource(baseName, tp);
        if ( resource.isValid()) {
            fileid = resource.id();
        } else {
            fileid = mastercatalog()->url2id(url, tp);
            if ( fileid == i64UNDEF) {
                kernel()->issues()->log(TR(ERR_MISSING_1).arg(completeName));
                fileid = i64UNDEF;
                return false;
            }
        }
    }
    return true;

}

QString ODFItem::findProjectionName() const {
   if(_ilwtype & itCONVENTIONALCOORDSYSTEM) {
        QString name = _odf.value("CoordSystem", "Projection");
        return cleanName(name);
   }
   return sUNDEF;
}

QString ODFItem::findDatumName() const {
    if(_ilwtype & itCONVENTIONALCOORDSYSTEM) {
        QString name = _odf.value("CoordSystem", "Datum");
        QString area = _odf.value("CoordSystem", "Datum Area");
        if ( name != sUNDEF) {
            name = Ilwis3Connector::name2Code(name + ( (area!= sUNDEF && area != "") ? "." + area : ""),"datum");
            return cleanName(name);
        }
    }
    return sUNDEF;
}

QString ODFItem::findDomainName(const QString& path) const
{
    quint64 validTypes = itTABLE | itCOVERAGE | itDOMAIN | itREPRESENTATION;
    if ( (_ilwtype & validTypes) == 0)
        return sUNDEF;

    QString name = sUNDEF;
    if(_ilwtype & itCOVERAGE) {
        QString ext = _file.suffix().toLower();
        if(ext != "mpl")
            name = _odf.value("BaseMap","Domain");
        else {
            QString rasmap = _odf.value("MapList","Map0");
            QFile file(rasmap) ;
            if ( !file.exists()) {
                rasmap = rasmap.remove("\'");
                rasmap = container().toLocalFile() + "/" + rasmap ;
            }
            if ( rasmap.indexOf(".mpr") == -1)
                rasmap += ".mpr";
            IniFile ini;
            ini.setIniFile(rasmap);
            name = ini.value("BaseMap","Domain");
        }
    }
    else if(_ilwtype & itTABLE)
        name = _odf.value("Table", "Domain");
    else if(_ilwtype & itDOMAIN)
        name = _file.fileName();
    else if(_ilwtype & itREPRESENTATION)
        name = _odf.value("Representation", "Domain");
    return cleanName(name);
}

IlwisTypes ODFItem::findDomainType(const QString& path) const
{
    quint64 validTypes = itTABLE | itCOVERAGE | itDOMAIN | itREPRESENTATION;
    if ( (_ilwtype & validTypes) == 0)
        return itUNKNOWN;

    if  ( _domname == "")
        return itUNKNOWN;

    if ( _domname == "UniqueID")
        return itITEMDOMAIN;
    if ( _domname == "bool.dom")
        return itNUMERICDOMAIN;
    if ( _domname == "value.dom")
        return itNUMERICDOMAIN;
    if ( _domname == "image.dom")
        return itNUMERICDOMAIN;
    if ( _domname == "color.dom")
        return itCOLORDOMAIN;

    //Resource resource = mastercatalog()->name2Resource(stripExtension(_domname),itDOMAIN);
    Resource resource = mastercatalog()->name2Resource(_domname,itDOMAIN);
    if ( resource.isValid())
        return resource.ilwisType();
    IniFile dm;
    QString localpath = container().toLocalFile() + "/" + _domname;
    if(!dm.setIniFile(localpath))
        return itUNKNOWN;

    QString type =  dm.value("Domain", "Type");
    if ( type == "DomainValue")
        return itNUMERICDOMAIN;
    if ( type == "DomainClass" )
        return itITEMDOMAIN;
    if ( type == "DomainIdentifier" || type == "DomainUniqueID")
        return itITEMDOMAIN;
    if ( type == "DomainCOORD" )
        return itCOORDSYSTEM;

    return itUNKNOWN;


}

QString ODFItem::findCsyName(const QString& path) const
{
    quint64 validTypes = itGEOREF | itCOORDSYSTEM | itCOVERAGE;
    if ( (_ilwtype & validTypes) == 0 )
        return sUNDEF;

    QString name = sUNDEF;
    if (_ilwtype & itCOVERAGE) {
        QString ext = _file.suffix().toLower();
        if(ext != "mpl")
            name = _odf.value("BaseMap","CoordSystem");
        else {
             QString grf = _odf.value("MapList","GeoRef");
             if ( grf != sUNDEF) {
                 grf = grf.remove("\'");
                 QFileInfo infgrf(grf);
                 if ( !infgrf.exists()) {
                     QString grfpath = container().toLocalFile() + "/" + grf;
                     if ( grfpath != "none.grf"){
                        IniFile ini;
                        ini.setIniFile(grfpath);
                        name = ini.value("GeoRef","CoordSystem");
                     }else {
                         name = "unknown.csy";
                     }
                 }
             }
        }
    }
    else if ( _ilwtype & itCOORDSYSTEM)
        name = _file.fileName();
    else if ( _ilwtype & itGEOREF)
        name = _odf.value("GeoRef","CoordSystem");

    return cleanName(name);
}

IlwisTypes ODFItem::findCsyType(const QString& path)
{
    quint64 validTypes = itGEOREF | itCOORDSYSTEM | itCOVERAGE;
    if ( (_ilwtype & validTypes) == 0)
        return itUNKNOWN;

    if ( _csyname == "")
        return itUNKNOWN;

    //Resource resource = mastercatalog()->name2Resource(stripExtension(_csyname),itCOORDSYSTEM);
    Resource resource = mastercatalog()->name2Resource(_csyname,itCOORDSYSTEM);
    if ( resource.isValid())
        return resource.ilwisType();
    IniFile csy;
    if ( _csyname == "latlonwgs84.csy"){

        if ( hasType(ilwisType(), itCOVERAGE))
            addProperty("latlonenvelope",_odf.value("BaseMap", "CoordBounds"));

        return itCONVENTIONALCOORDSYSTEM;
    }
    if ( _csyname == "unknown.csy")
        return itBOUNDSONLYCSY;

    QString ext = _file.suffix().toLower();
    if(ext == "mpl") {
        QString grf = _odf.value("MapList","GeoRef");
        if ( grf != sUNDEF) {
            QString csyname;
            QFileInfo infgrf(grf);
            if ( !infgrf.exists()) {
                grf = grf.remove("\'");
                grf = container().toLocalFile()+ "/" + grf;
            }
            IniFile ini;
            ini.setIniFile(grf);
            csyname = ini.value("GeoRef","CoordSystem");
            QFile file(csyname);
            if ( !file.exists()){
                csyname = csyname.remove("\'");
                csyname = container().toLocalFile() + "/" + csyname;
            }
            csy.setIniFile(csyname);

        }
    } else {
        QString path = _csyname;
        QFile file(_csyname);
        if ( !file.exists())
            path = container().toLocalFile() + "/" + path;
        else{
            QFileInfo inf (_csyname);
            path = inf.absoluteFilePath();
        }

        if(!csy.setIniFile(path)){
            return itUNKNOWN;
        }
    }

    QString type = csy.value("CoordSystem", "Type");
    if ( type.compare("latlon",Qt::CaseInsensitive) == 0){
        addProperty("projectionname", "LatLon WGS 84");
        return itCONVENTIONALCOORDSYSTEM;
    }
    if ( type.compare("boundsonly",Qt::CaseInsensitive) == 0)
        return itBOUNDSONLYCSY;

    // type empty type can happen due to bug in older version of ilwis. no type was projected
    if ( type == sUNDEF || type.compare("projection",Qt::CaseInsensitive) == 0) {
        QString proj = csy.value("CoordSystem","Projection");
        if ( proj != sUNDEF){
            if ( proj.toLower() == "utm"){
                QString z = csy.value("Projection","Zone");
                proj += " " + z;
            }
            addProperty("projectionname", proj);
        }
        quint64 tp = itCONVENTIONALCOORDSYSTEM ;
        return tp;
    }
    return itUNKNOWN;

}

QString ODFItem::findGrfName() const{
    quint64 validTypes = itGEOREF | itRASTER;
    if ( (_ilwtype & validTypes) == 0)
        return sUNDEF;

    QString name = sUNDEF;

    if ( _ilwtype & itRASTER) {
        name = _odf.value("Map","GeoRef");
        if ( name == sUNDEF)
            name = _odf.value("MapList","GeoRef");
    }
    else if ( _ilwtype & itGEOREF)
        name = _file.fileName();
    return cleanName(name);

}

quint64 ODFItem::findSize() const
{
    return objectSize();
}

quint64 ODFItem::partSize(const QUrl& file, const QString& section, const QString& key)  const{
    QFileInfo part = file.toLocalFile();
    if ( section != "") {
        IniFile odf;
        odf.setIniFile(file.toLocalFile());
        QString filename = odf.value(section,key);
        if ( !filename.contains(QRegExp("\\\\|/"))) {
            //TODO: changes this for files that are in non folder containers
            filename = part.absolutePath() + "/" + filename;
            part = QFileInfo(filename);
        }

    }
    QFileInfo inf(part.absoluteFilePath());
    return inf.size();

}

quint64 ODFItem::objectSize() const {

    qint64 sz = partSize(_odf.file(),"","");
    bool versionOk;
    double rVersion = _odf.value("Ilwis", "Version").toDouble(&versionOk);

    switch(_ilwtype )
    {
    case itRASTER:
        sz += partSize(_odf.file(), "MapStore", "Data"); break;
    case itTABLE:
        sz += partSize(_odf.file(), "TableStore", "Data"); break;
    case itPOINT:
        sz += partSize(_odf.file(), "TableStore", "Data"); break;
    case itLINE:
        if ( rVersion >= 3.0)
        {
            sz += partSize(_odf.file(), "TableStore", "Data");
            sz += partSize(_odf.file(), "ForeignFormat", "Filename");
        }
        else
        {
            sz += partSize(_odf.file(), "SegmentMapStore", "DataSeg");
            sz += partSize(_odf.file(), "SegmentMapStore", "DataSegCode");
            sz += partSize(_odf.file(), "SegmentMapStore", "DataCrd");
        }
        break;
    case itPOLYGON:
        if ( rVersion >= 3.0)
        {
            sz += partSize(_odf.file(), "top:TableStore", "Data");
            sz += partSize(_odf.file(), "TableStore", "Data");
            sz += partSize(_odf.file(), "ForeignFormat", "Filename");
        }
        else
        {
            sz += partSize(_odf.file(), "SegmentMapStore", "DataSeg");
            sz += partSize(_odf.file(), "SegmentMapStore", "DataCrd");
            sz += partSize(_odf.file(), "PolygonMapStore", "DataPol");
            sz += partSize(_odf.file(), "PolygonMapStore", "DataPolCode");
            sz += partSize(_odf.file(), "PolygonMapStore", "DataTop");
        }
        break;
    case itDOMAIN:
        sz += partSize(_odf.file(), "TableStore", "Data"); break; // only true for Domainssort
    case itGEOREF:
        sz += partSize(_odf.file(), "TableStore", "Data"); break;
    case itCOORDSYSTEM:
        sz += partSize(_odf.file(), "TableStore", "Data"); break;

        if ( _file.suffix() == "isl")
            sz += partSize(_odf.file(), "Script", "ScriptFile"); break;
        if ( _file.suffix() == "fun")
            sz += partSize(_odf.file(), "FuncUser", "FuncDeffile"); break;
    }
    return sz;
}

QString ODFItem::findDimensions() const
{
    switch( _ilwtype)
    {
        case itGEOREF:
        {
            QString sX = _odf.value("GeoRef", "Columns");
            if ( sX == "") return "";
            QString sY = _odf.value( "GeoRef", "Lines");
            if ( sY == "") return "";
            return QString("%1 x %2").arg(sX, sY);
        }
        case itRASTER:
        {
            QString sSize = _odf.value( "Map", "Size");
            QStringList xy = sSize.split(" ");
            if ( xy.size() != 2)
                return "";

            return QString ("%1 x %2").arg( xy[0], xy[1]);
        }
        case itLINE:
            return _odf.value( "SegmentMapStore", "Segments");
        case itPOLYGON:
            return _odf.value( "PolygonMapStore", "Polygons");
        case itPOINT:
            return _odf.value( "PointMap", "Points");
        case itCOORDSYSTEM:
        case itCONVENTIONALCOORDSYSTEM:
        case itBOUNDSONLYCSY:
            {
                QString sV = "";
                QString bnds = _odf.value("CoordSystem", "CoordBounds");
                QStringList parts = bnds.split(" ");
                if ( parts.size() != 4)
                    return "";
                if ( parts[0] == "1e+030" || parts[0] == "-1e+308")
                    return "";

                if ( parts[0].toDouble() > -180 && parts[0].toDouble() < 180)
                    sV = QString("%1 x %2 x %3 x %4").arg(parts[0].toDouble(),0,'g',3).arg(parts[1].toDouble(),0,'g',3).arg(parts[2].toDouble(),0,'g',3).arg(parts[3].toDouble(),0,'g',3);
                else
                    sV = QString("%1 x %2 x %3 x %4").arg(parts[0]).arg(parts[1]).arg(parts[2]).arg(parts[3]);
                return sV;
            }
        case itDOMAIN:
        case itITEMDOMAIN:
            {
                QString dtype =  _odf.value("Domain","Type");
                if ( dtype == "DomainClass" || dtype == "DomainGroup")
                    return _odf.value("DomainClass", "Nr");
                else if ( dtype == "DomainIdentifier" || dtype == "DomainUniqueID")
                    return _odf.value("DomainIdentifier", "Nr");
            }
        case itNUMERICDOMAIN:
            return _odf.value( "Table", "Records");
        case itTABLE:
            return QString ( "%1 x %2").arg( _odf.value( "Table", "Records"), _odf.value( "Table", "Columns"));
    }

    return "";
}

QString ODFItem::stripExtension(const QString& name) const{
    QFileInfo inf(name);
    return inf.baseName();
}

bool ODFItem::isSystemObject(const QString& name) {
    QString temp = QFileInfo(name).baseName().toLower();
    return systemObjectNames.indexOf(name) != -1;

}

QString  ODFItem::cleanName(const QString& name) const{
    if ( name.size() == 0)
        return name;
    QString cleanTxt = name.trimmed();
    if ( cleanTxt.contains(QRegExp("\\\\|/"))) {
        cleanTxt = QFileInfo(cleanTxt).fileName();
    }
    if ( cleanTxt.at(0) == '\'') {
        cleanTxt = cleanTxt.remove("'");
    }
    return cleanTxt.toLower();
}
