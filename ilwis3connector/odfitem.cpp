#include <QRegExp>
#include <QFileInfo>
#include <QUrl>
#include <QStringList>
#include <QDir>
#include "kernel.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "inifile.h"
#include "identity.h"
#include "resource.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "odfitem.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"

const QString Ilwis::Ilwis3::ODFItem::systemObjectNames="value;image;image;min1to1;nilto1;count;distance;ndvi;percentage;none;latlonwgs84;latlon;bool;byte \
        color;colorcmp";

using namespace Ilwis;
using namespace Ilwis3;

#include <typeinfo>

ODFItem::ODFItem(const QUrl &file, const UPContainerConnector& container) : Resource(file,itANY), _projectionName(sUNDEF)
{
    _odf.setIniFile(file, container);
    _file = container->toLocalFile(file);
    setName(_file.fileName(), false);
    addContainer(QUrl::fromLocalFile(_file.canonicalPath()));
    IlwisTypes csytp, domtp, grftp;
    csytp = domtp = grftp = itUNKNOWN;

    _ilwtype = Ilwis3Connector::ilwisType(_file.absoluteFilePath());
    _csyname = findCsyName(container);
    _domname = findDomainName(container);
    _grfname = findGrfName();
    _datumName = findDatumName();


    csytp = findCsyType(container);
    domtp = findDomainType(container);


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

    _size = findSize(container);
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
        if ( value == "latlonwgs84.csy") {
            Resource resource = mastercatalog()->name2Resource("code=epsg:4326", itCOORDSYSTEM);
            if ( !resource.isValid()) {
               return ERROR1(ERR_FIND_SYSTEM_OBJECT_1, "Wgs 84");
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

QString ODFItem::findDomainName(const UPContainerConnector& containerc) const
{
    quint64 validTypes = itTABLE | itCOVERAGE | itDOMAIN;
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
                rasmap = container().toString() + "/" + rasmap ;
            }
            if ( rasmap.indexOf(".mpr") == -1)
                rasmap += ".mpr";
            IniFile ini;
            ini.setIniFile(rasmap, containerc);
            name = ini.value("BaseMap","Domain");
        }
    }
    else if(_ilwtype & itTABLE)
        name = _odf.value("Table", "Domain");
    else if(_ilwtype & itDOMAIN)
        name = _file.fileName();
    return cleanName(name);
}

IlwisTypes ODFItem::findDomainType(const UPContainerConnector& containerc) const
{
    quint64 validTypes = itTABLE | itCOVERAGE | itDOMAIN;
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

    //Resource resource = mastercatalog()->name2Resource(stripExtension(_domname),itDOMAIN);
    Resource resource = mastercatalog()->name2Resource(_domname,itDOMAIN);
    if ( resource.isValid())
        return resource.ilwisType();
    IniFile dm;
    QString path = container().toString() + "/" + _domname;
    if(!dm.setIniFile(path, containerc))
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

QString ODFItem::findCsyName(const UPContainerConnector& containerc) const
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
                 QFileInfo infgrf(grf);
                 if ( !infgrf.exists()) {
                     QString grfpath = container().toString() + "/" + grf;
                     IniFile ini;
                     ini.setIniFile(grfpath, containerc);
                     name = ini.value("GeoRef","CoordSystem");
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

IlwisTypes ODFItem::findCsyType(const UPContainerConnector& containerc) const
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
    if ( _csyname == "latlonwgs84.csy")
        return itCONVENTIONALCOORDSYSTEM;

    QString ext = _file.suffix().toLower();
    if(ext == "mpl") {
        QString grf = _odf.value("MapList","GeoRef");
        if ( grf != sUNDEF) {
            QString csyname;
            QFileInfo infgrf(grf);
            if ( !infgrf.exists()) {
                grf = container().toString()+ "/" + grf;
            }
            IniFile ini;
            ini.setIniFile(grf, containerc);
            csyname = ini.value("GeoRef","CoordSystem");
            QFile file(csyname);
            if ( !file.exists())
                csyname = container().toString() + "/" + csyname;
            csy.setIniFile(csyname, containerc);

        }
    } else {
        QString path = _csyname;
        QFile file(path);
        if ( !file.exists())
            path = container().toString() + "/" + path;

        if(!csy.setIniFile(path, containerc)){
            return itUNKNOWN;
        }
    }

    QString type = csy.value("CoordSystem", "Type");
    if ( type.compare("latlon",Qt::CaseInsensitive) == 0)
        return itCONVENTIONALCOORDSYSTEM;

    // type empty type can happen due to bug in older version of ilwis. no type was projected
    if ( type == sUNDEF || type.compare("projection",Qt::CaseInsensitive) == 0) {
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

//IlwisTypes ODFItem::findGrfType() const {

//    quint64 validTypes = itGEOREF | itRASTER;
//    if ( (_ilwtype & validTypes) == 0)
//        return itUNKNOWN;

//    if ( _grfname == "")
//        return itUNKNOWN;
//    Resource resource = mastercatalog()->name2Resource(stripExtension(_grfname),itGEOREF);
//    if ( resource.isValid())
//        return resource.ilwisType();
//    IniFile grf;
//    QString path = _file.canonicalPath() + "/" + _grfname;
//    if(!grf.setIniFile(path))
//        return itUNKNOWN;

//    QString type =  grf.value("GeoRef", "Type");
//    if ( type.compare("georefcorners",Qt::CaseInsensitive) == 0)
//        return itCORNERSGEOREF;
//    if ( type.compare("georefctp",Qt::CaseInsensitive) == 0)
//        return itTIEPOINTGEOREF;

//    return itUNKNOWN;
//}

quint64 ODFItem::findSize(const UPContainerConnector& container) const
{
    return objectSize(container);
//    if ( sz == 0)
//        return "";
//     if ( sz < 1000)
//         return QString("%1 bytes").arg(sz);
//     if ( sz < 1e6)
//         return QString("%1 kB").arg(sz / 1000.0,0,'f',2);
//     if ( sz < 1e9)
//         return QString("%1 MB").arg(sz / 1e6,0,'f',2);

//     return QString("%1 GB").arg(sz / 1e9,0,'f',2);


}

quint64 ODFItem::partSize(const QUrl& file, const QString& section, const QString& key,const UPContainerConnector& containerc)  const{
    QFileInfo part = containerc->toLocalFile(file);
    if ( section != "") {
        IniFile odf;
        odf.setIniFile(file, containerc);
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

quint64 ODFItem::objectSize(const UPContainerConnector& container) const {

    qint64 sz = partSize(_odf.file(),"","", container);
    bool versionOk;
    double rVersion = _odf.value("Ilwis", "Version").toDouble(&versionOk);

    switch(_ilwtype )
    {
    case itRASTER:
        sz += partSize(_odf.file(), "MapStore", "Data", container); break;
    case itTABLE:
        sz += partSize(_odf.file(), "TableStore", "Data", container); break;
    case itPOINT:
        sz += partSize(_odf.file(), "TableStore", "Data", container); break;
    case itLINE:
        if ( rVersion >= 3.0)
        {
            sz += partSize(_odf.file(), "TableStore", "Data", container);
            sz += partSize(_odf.file(), "ForeignFormat", "Filename", container);
        }
        else
        {
            sz += partSize(_odf.file(), "SegmentMapStore", "DataSeg", container);
            sz += partSize(_odf.file(), "SegmentMapStore", "DataSegCode", container);
            sz += partSize(_odf.file(), "SegmentMapStore", "DataCrd", container);
        }
        break;
    case itPOLYGON:
        if ( rVersion >= 3.0)
        {
            sz += partSize(_odf.file(), "top:TableStore", "Data", container);
            sz += partSize(_odf.file(), "TableStore", "Data", container);
            sz += partSize(_odf.file(), "ForeignFormat", "Filename", container);
        }
        else
        {
            sz += partSize(_odf.file(), "SegmentMapStore", "DataSeg", container);
            sz += partSize(_odf.file(), "SegmentMapStore", "DataCrd", container);
            sz += partSize(_odf.file(), "PolygonMapStore", "DataPol", container);
            sz += partSize(_odf.file(), "PolygonMapStore", "DataPolCode", container);
            sz += partSize(_odf.file(), "PolygonMapStore", "DataTop", container);
        }
        break;
    case itDOMAIN:
        sz += partSize(_odf.file(), "TableStore", "Data", container); break; // only true for Domainssort
    case itGEOREF:
        sz += partSize(_odf.file(), "TableStore", "Data", container); break;
    case itCOORDSYSTEM:
        sz += partSize(_odf.file(), "TableStore", "Data", container); break;

        if ( _file.suffix() == "isl")
            sz += partSize(_odf.file(), "Script", "ScriptFile", container); break;
        if ( _file.suffix() == "fun")
            sz += partSize(_odf.file(), "FuncUser", "FuncDeffile", container); break;
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
            {
                QString sV = "";
                QString bnds = _odf.value("CoordSystem", "CoordBounds");
                QStringList parts = bnds.split(" ");
                if ( parts.size() != 4)
                    return "";
                sV = QString("%1 %2 %3 %4").arg(parts[0], parts[1], parts[2], parts[3]);
                return sV;
            }
        case itDOMAIN:
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
