#include <QSqlQuery>
#include <QSqlError>
#include <QRegExp>

#include "kernel.h"
#include "raster.h"
#include "module.h"
#include "featurecoverage.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "columndefinition.h"
#include "table.h"
#include "rawconverter.h"
#include "ilwis3connector.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "tableconnector.h"
#include "coordinatesystemconnector.h"
#include "coverageconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

CoverageConnector::CoverageConnector(const Resource &resource, bool load, const IOOptions &options) : Ilwis3Connector(resource, load, options)
{
}

bool CoverageConnector::getRawInfo(const QString& range, double& vmin, double& vmax, double& scale, double& offset) const{
    QStringList parts = range.split(":");
    if ( parts.size() >= 2) {
        bool ok1, ok2;
        vmin = parts[0].toDouble(&ok1);
        vmax = parts[1].toDouble(&ok2);
        if ( (ok1 && ok2) == false) {
            return ERROR1(ERR_ILLEGAL_VALUE_RANGE_DEFINITION_1,_resource.name());
        }
        scale = 1.0;
        offset = 0;
        if ( parts.size() == 3) {
            offset = parts[2].mid(7).toDouble();
        } if ( parts.size() == 4) {
            scale = parts[2].toDouble();
            offset = parts[3].mid(7).toDouble();
        }
         const_cast<CoverageConnector *>(this)->_converter = RawConverter(offset, scale, vmin, vmax);
         return true;
    }
    return false;
}

ITable CoverageConnector::prepareAttributeTable(const QString& file, const QString& basemaptype, const IOOptions &options) const{

    ITable attTable;
    if ( file != sUNDEF) {
        QString newfile = filename2FullPath(file, this->_resource);
        if(!attTable.prepare(newfile)){
            kernel()->issues()->log(file,TR(ERR_NO_INITIALIZED_1).arg(file),IssueObject::itWarning);
            return attTable;
        }
    }

    if ( basemaptype != "Map" ) {
        QString name = _resource.name();
        int index = -1;
        if ( (index = name.indexOf(".")) != -1){
            name = name.left(index);
        }
    }
    if ( !attTable.isValid())
        attTable.prepare();

    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;
    if ( isNumeric){
        IDomain featuredom("value");
        attTable->addColumn(FEATUREVALUECOLUMN,featuredom);
    }
    else if ( attTable->columnIndex(COVERAGEKEYCOLUMN) == iUNDEF) { // external tables might already have these
        DataDefinition def = determineDataDefintion(_odf, options);
        attTable->addColumn(ColumnDefinition(COVERAGEKEYCOLUMN,def, attTable->columnCount()));
    }
    return attTable;

}

bool CoverageConnector::loadMetaData(Ilwis::IlwisObject *data,const IOOptions& options)
{
    Ilwis3Connector::loadMetaData(data, options);

    Coverage *coverage = static_cast<Coverage *>(data);
    QString csyName = _odf->value("BaseMap","CoordSystem");
    if ( csyName == sUNDEF){
        //due to inconsistent spelling we have to check other cases
        csyName = _odf->value("BaseMap","Coordsystem");
    }
    if ( csyName.toLower() == "latlonwgs84.csy")
        csyName = "code=epsg:4326";
    else if ( csyName.toLower() == "unknown.csy")
        csyName = "code=csy:unknown";
    else{
        csyName = filename2FullPath(csyName, this->_resource);
    }
    ICoordinateSystem csy;
    if ( !csy.prepare(csyName, itCOORDSYSTEM, options)) {
        kernel()->issues()->log(csyName,TR("Coordinate system couldnt be initialized, defaulting to 'unknown'"),IssueObject::itWarning);
        QString resource = QString("code=csy:unknown");
        if (!csy.prepare(resource)) {
            kernel()->issues()->log(TR("Fallback to 'unknown' failed, corrupt system files defintion"));
            return false;
        }
    }
    coverage->coordinateSystem(csy);


    QString attfile = _odf->value("BaseMap", "AttributeTable");
    QString basemaptype = _odf->value("BaseMap", "Type");
    // feature coverages always have an attribute table; rasters might have
    if ( basemaptype != "Map" || attfile != sUNDEF) {
        ITable tbl = prepareAttributeTable(attfile, basemaptype, options);
        if ( tbl.isValid()){
        if ( basemaptype == "Map"){
            static_cast<RasterCoverage *>(coverage)->attributeTable(tbl);
        } else
            static_cast<FeatureCoverage *>(coverage)->attributesFromTable(tbl);
        }
    }

    QString cbounds = _odf->value("BaseMap","CoordBounds");
    if ( cbounds == sUNDEF) // possible inconsistent spelling
        cbounds = _odf->value("BaseMap","Coordbounds");

    QStringList parts = cbounds.split(" ", QString::SkipEmptyParts);
    if ( parts.size() == 4) {
        double minx = parts[0].toDouble();
        double miny = parts[1].toDouble();
        double maxx = parts[2].toDouble();
        double maxy = parts[3].toDouble();
        Envelope env(Coordinate(minx, miny), Coordinate(maxx, maxy));
        coverage->envelope(env);
    } else {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Coordinate boundaries", data->name()), IssueObject::itWarning);
    }


    return true;
}

bool CoverageConnector::storeMetaData(IlwisObject *obj, IlwisTypes type, const Ilwis::IDomain &dom, const QString& baseName)
{
    if (!dom.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "Domain", obj->name());

    bool ok = Ilwis3Connector::storeMetaData(obj, type);
    if ( !ok)
        return false;

    Coverage *coverage = static_cast<Coverage *>(obj);

    const ICoordinateSystem csy = coverage->coordinateSystem();
    if (!csy.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "CoordinateSystem", coverage->name());

    // create a suitable filepath
    if ( csy->code() != "unknown"){
        _csyName = Resource::toLocalFile(csy->source().url(),true, "csy");
        if ( csy->isInternalObject()){
            QString csyFile = Resource::toLocalFile(source().url(),false, "csy");
            int index = csy->source().url().toString().lastIndexOf("/");
            QString name = csy->source().url().toString().mid(index + 1);
            _csyName =  QFileInfo(csyFile).absolutePath() + "/" + name;
        }
        else if ( _csyName == sUNDEF || _csyName == "") {
            QString path = context()->workingCatalog()->filesystemLocation().toLocalFile() + "/";
            QString name = csy->name();
            if ( !csy->isAnonymous()) {
                name = name.replace(QRegExp("[/ .'\"]"),"_");
            }
            _csyName = path + name;
            if ( !_csyName.endsWith(".csy"))
                _csyName += ".csy";
            //return ERROR2(ERR_NO_INITIALIZED_2, "CoordinateSystem", coverage->name());
        }

        QFileInfo csyinf(_csyName);
        if ( !csyinf.exists()) { // if filepath doesnt exist we create if from scratch
            if (!csyinf.isAbsolute()){
                _csyName = context()->workingCatalog()->filesystemLocation().toLocalFile() + "/" + _csyName;
            }

            QUrl url = QUrl::fromLocalFile(_csyName); // new attempt to create a suitable path;
            csy->connectTo(url,"coordsystem","ilwis3", IlwisObject::cmOUTPUT);
            if(!csy->store({"storemode",Ilwis::IlwisObject::smMETADATA})){ // fail, we default to unknown
                _csyName = "Unknown.csy";
                WARN2(ERR_NO_INITIALIZED_2,"CoordinateSystem",obj->name());
            } else {
                _csyName = url.toLocalFile();
            }
        }
        _odf->setKeyValue("BaseMap","CoordSystem", QFileInfo(_csyName).fileName());
    }else
        _odf->setKeyValue("BaseMap","CoordSystem", "unknown.csy");

    Envelope bounds = coverage->envelope();
    if ( bounds.isNull())
        bounds = coverage->coordinateSystem()->envelope();
    if(!bounds.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "Bounds", coverage->name());

    _odf->setKeyValue("BaseMap","CoordBounds",QString("%1 %2 %3 %4").
                      arg(bounds.min_corner().x,0,'f').
                      arg(bounds.min_corner().y,0,'f').
                      arg(bounds.max_corner().x,0,'f').
                      arg(bounds.max_corner().y,0,'f'));



    calcStatics(obj,NumericStatistics::pBASIC);

    if ( dom->ilwisType() == itNUMERICDOMAIN) {

        quint16 digits = coverage->statistics().significantDigits();
        qint32 delta = coverage->statistics()[NumericStatistics::pDELTA];
        _domainName = "value.dom";
        if ( delta >= 0 && delta < 256 && digits == 0){
            if ( delta >= 0 && delta < 256 && digits == 0){
                if ( dom->code() == "boolean"){
                    _domainName = "bool";
                    _domainInfo = QString("bool.dom;Byte;bool;0;;");
                    _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
                    _odf->setKeyValue("BaseMap","Range","0:1:offset=-1");
                    _odf->setKeyValue("BaseMap","Domain","bool.dom");
                }
                else{
                    _domainName = "image.dom";
                    _domainInfo = QString("Image.dom;Byte;image;0;;");
                    _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
                    _odf->setKeyValue("BaseMap","Range","0:255:offset=0");
                    _odf->setKeyValue("BaseMap","MinMax","0:255");
                    _odf->setKeyValue("BaseMap","Domain","Image.dom");
                }
            }
        }
        else {
            const NumericStatistics& stats = coverage->statistics();
            int digits = stats.significantDigits();
            RawConverter conv(stats[NumericStatistics::pMIN], stats[NumericStatistics::pMAX],pow(10, - digits));
            _domainInfo = QString("%1:%2:%3:offset=%4").arg(stats[NumericStatistics::pMIN]).arg(stats[NumericStatistics::pMAX]).arg(conv.scale()).arg(conv.offset());
            _odf->setKeyValue("BaseMap","Range",_domainInfo);
            _odf->setKeyValue("BaseMap","Domain","value.dom");

            _odf->setKeyValue("BaseMap","MinMax",QString("%1:%2").arg(stats[NumericStatistics::pMIN]).arg(stats[NumericStatistics::pMAX]));
            QString _domainInfo = QString("value.dom;Long;value;0;-9999999.9:9999999.9:0.1:offset=0");
            _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
        }
    } if ( dom->ilwisType() == itITEMDOMAIN) {
         if ( hasType(dom->valueType(),itTHEMATICITEM | itNUMERICITEM) && coverage->ilwisType() == itRASTER) {
            _domainName =  Resource::toLocalFile(dom->source().url(), true);
            if ( _domainName == sUNDEF){
                _domainName = QFileInfo(baseName).baseName() + ".dom";
            }
            IThematicDomain themdom = dom.as<ThematicDomain>();
            if ( themdom.isValid()) {
                _domainInfo = QString("%1;Byte;class;%2;;").arg(_domainName).arg(themdom->count());
                _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
                _odf->setKeyValue("BaseMap","Domain",_domainName);
            }
            _itemCount = themdom->count();
        } else if(dom->valueType() == itINDEXEDITEM) {
            _domainName = QFileInfo(QUrl(_odf->file()).toLocalFile()).fileName();
            _domainInfo = QString("%1;Long;UniqueID;0;;").arg(_domainName);
            _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
            _odf->setKeyValue("BaseMap","Domain",_domainName);
        } else if ( dom->valueType() == itNAMEDITEM) {
            INamedIdDomain iddom = dom.as<NamedIdDomain>();
            _domainName = Resource::toLocalFile(dom->source().url(), true);
            if ( _domainName == sUNDEF){
                _domainName = QFileInfo(baseName).baseName() + ".dom";
            }
            _domainInfo = QString("%1;Int;id;%2;;").arg(_domainName).arg(iddom->count());
            _odf->setKeyValue("BaseMap","DomainInfo",_domainInfo);
            _odf->setKeyValue("BaseMap","Domain",_domainName);
            QUrl url = makeUrl(_odf->file(),_domainName);
            iddom->connectTo(url,"domain","ilwis3", IlwisObject::cmOUTPUT);
            iddom->store({"storemode",Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA});
            _itemCount = iddom->count();
        }
    }
    QFileInfo inf(_domainName);
    _domainName = inf.fileName();

    return true;
}



TableConnector *CoverageConnector::createTableStoreConnector(ITable& attTable, Coverage *coverage, IlwisTypes tp, const QString& baseName) {
    QString dataFile = baseName;
    QString attDom = dataFile;
    if ( hasType(tp,itRASTER)) {
        RasterCoverage *raster = static_cast<RasterCoverage *>(coverage);
        Resource resource = raster->datadef().domain<>()->source();
        QFileInfo inf(resource.toLocalFile());
        attDom = inf.fileName();
    }
    int index = dataFile.lastIndexOf(".");
    if ( index != -1) {
        dataFile = dataFile.left(index);
    }else{
        if ( tp == itPOLYGON)
            attDom += ".mpa";
        if ( tp == itRASTER)
            attDom += ".mpr";
        if ( tp == itPOINT)
            attDom += ".mpp";
        if ( tp == itLINE)
            attDom += ".mps";
    }
    if ( attTable->columnCount() > 1) { // one column means only featurid which we dont save.
        QFileInfo inf(dataFile);
        QString filename = dataFile + ".tbt";
        _odf->setKeyValue("BaseMap","AttributeTable",filename);
//        attTable->setName(inf.baseName()); // why should we chance the name of the table to a local filename here ?
        TableConnector *conn = new TableConnector(Resource(QUrl::fromLocalFile(filename), itTABLE), false);
        conn->attributeDomain(attDom);

        return conn;
    }
    return 0;
}
DataDefinition CoverageConnector::determineDataDefintion(const ODF& odf,  const IOOptions &options) const{
    IDomain dom;
    QString domname = odf->value("BaseMap","Domain");
    QString filename = context()->workingCatalog()->resolve(domname, itDOMAIN);
    if(!dom.prepare(filename, options)) {
        ERROR2(ERR_NO_INITIALIZED_2,"domain",odf->file());
        return DataDefinition();
    }

    DataDefinition def(dom);
    double vmax,vmin,scale,offset;
    QString range = odf->value("BaseMap","Range");
    if ( range != sUNDEF ) {
        if ( getRawInfo(range, vmin,vmax,scale,offset)) {
            if ( scale == 1.0) {
                def.range(new NumericRange(vmin, vmax,1));

            }
            else {
                def.range(new NumericRange(vmin, vmax));
            }


        }
    }
    return def;
}
