#include <QSqlQuery>
#include <QSqlError>
#include <QRegExp>

#include "kernel.h"
#include "raster.h"
#include "module.h"
#include "connectorinterface.h"
#include "containerconnector.h"
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
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "tableconnector.h"
#include "coordinatesystemconnector.h"
#include "coverageconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

CoverageConnector::CoverageConnector(const Resource &resource, bool load) : Ilwis3Connector(resource, load)
{
}

bool CoverageConnector::getRawInfo(const QString& range, double& vmin, double& vmax, double& scale, double& offset) {
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
         _converter = RawConverter(offset, scale, vmin, vmax);
         return true;
    }
    return false;
}

ITable CoverageConnector::prepareAttributeTable(const QString& file, const QString& basemaptype) const{

    ITable extTable;
    if ( file != sUNDEF) {
        QString newfile = filename2FullPath(file, this->_resource);
        if(!extTable.prepare(newfile)){
            kernel()->issues()->log(file,TR(ERR_NO_INITIALIZED_1).arg(file),IssueObject::itWarning);
            return ITable();
        }
    }

    IDomain covdom;
    if (!covdom.prepare("count")){
        return ITable();
    }

    ITable attTable;
    if ( basemaptype != "Map" ) {
        Resource resource(QUrl(QString("ilwis://internalcatalog/%1").arg(_resource.name())), itFLATTABLE);
        if(!attTable.prepare(resource)) {
            ERROR1(ERR_NO_INITIALIZED_1,resource.name());
            return ITable();
        }
        if ( extTable.isValid()) {
            for(int i=0; i < extTable->columnCount(); ++i) {
                attTable->addColumn(extTable->columndefinition(i));
            }
        }
    } else {
        attTable = extTable;
    }
    if ( attTable->columnIndex(FEATUREIDCOLUMN) == iUNDEF) { // external tables might already have these
        attTable->addColumn(COVERAGEKEYCOLUMN,covdom);
        attTable->addColumn(FEATUREIDCOLUMN,covdom);
    }

    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;
    if ( isNumeric) {
        IDomain featuredom;
        if (!featuredom.prepare("value")){
            return ITable();
        }
        attTable->addColumn(FEATUREVALUECOLUMN,featuredom);
    }
    return attTable;

}

bool CoverageConnector::loadMetaData(Ilwis::IlwisObject *data)
{
    Ilwis3Connector::loadMetaData(data);

    Coverage *coverage = static_cast<Coverage *>(data);
    QString csyName = _odf->value("BaseMap","CoordSystem");
    if ( csyName.toLower() == "latlonwgs84.csy")
        csyName = "code=epsg:4326";
    else{
        csyName = filename2FullPath(csyName, this->_resource);
    }
    ICoordinateSystem csy;
    if ( !csy.prepare(csyName, itCONVENTIONALCOORDSYSTEM)) {
        kernel()->issues()->log(csyName,TR("Coordinate system couldnt be initialized, defaulting to 'unknown'"),IssueObject::itWarning);
        QString resource = QString("ilwis://file/unknown.csy");
        if (!csy.prepare(resource)) {
            kernel()->issues()->log(TR("Fallback to 'unknown failed', corrupt system files defintion"));
            return false;
        }
    }
    coverage->setCoordinateSystem(csy);


    QString attfile = _odf->value("BaseMap", "AttributeTable");
    QString basemaptype = _odf->value("BaseMap", "Type");
    // feature coverages always have an attribute table; rasters might have
    if ( basemaptype != "Map" || attfile != sUNDEF) {
        ITable attTable = prepareAttributeTable(attfile, basemaptype);
        if (!attTable.isValid())
            return false;

        coverage->attributeTable(attTable);
    }

    QString cbounds = _odf->value("BaseMap","CoordBounds");
    QStringList parts = cbounds.split(" ");
    if ( parts.size() == 4) {
        double minx = parts[0].toDouble();
        double miny = parts[1].toDouble();
        double maxx = parts[2].toDouble();
        double maxy = parts[3].toDouble();
        Box2D<double> env(Coordinate(minx, miny), Coordinate(maxx, maxy));
        coverage->envelope(env);
    } else {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Coordinate boundaries", data->name()), IssueObject::itWarning);
    }


    return true;
}

bool CoverageConnector::storeMetaData(IlwisObject *obj, IlwisTypes type, const DataDefinition& datadef)
{
    bool ok = Ilwis3Connector::storeMetaData(obj, type);
    if ( !ok)
        return false;

    Coverage *coverage = static_cast<Coverage *>(obj);

    const ICoordinateSystem csy = coverage->coordinateSystem();
    if (!csy.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "CoordinateSystem", coverage->name());

    QString localName = Resource::toLocalFile(csy->source().url(),true);
    if ( localName == sUNDEF) {
        localName = CoordinateSystemConnector::createCsyFromCode(csy->code());
    }
    if ( localName == sUNDEF) {
        return ERROR2(ERR_NO_INITIALIZED_2, "CoordinateSystem", coverage->name());
    }
    _odf->setKeyValue("BaseMap","CoordSystem", localName);
    Box2D<double> bounds = coverage->envelope();
    if(!bounds.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "Bounds", coverage->name());

    _odf->setKeyValue("BaseMap","CoordBounds",QString("%1 %2 %3 %4").
                      arg(bounds.min_corner().x(),10,'f').
                      arg(bounds.min_corner().y(),10,'f').
                      arg(bounds.max_corner().x(),10,'f').
                      arg(bounds.max_corner().y(),10,'f'));

    const IDomain dom = datadef.domain();
    if (!dom.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "Domain", coverage->name());

    calcStatics(obj,NumericStatistics::pBASIC);
    if ( dom->ilwisType() == itNUMERICDOMAIN) {

        quint16 digits = coverage->statistics().significantDigits();
        qint32 delta = coverage->statistics()[NumericStatistics::pDELTA];
        if ( delta >= 0 && delta < 256 && digits == 0){
            if ( delta >= 0 && delta < 256 && digits == 0){
                if ( datadef.domain()->code() == "boolean"){
                    QString domInfo = QString("bool.dom;Byte;bool;0;;");
                    _odf->setKeyValue("BaseMap","DomainInfo",domInfo);
                    _odf->setKeyValue("BaseMap","Range","0:1:offset=-1");
                    _odf->setKeyValue("BaseMap","Domain","bool.dom");
                }
                else{
                    QString domInfo = QString("Image.dom;Byte;image;0;;");
                    _odf->setKeyValue("BaseMap","DomainInfo",domInfo);
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
            QString rangeString = QString("%1:%2:%3:offset=%4").arg(stats[NumericStatistics::pMIN]).arg(stats[NumericStatistics::pMAX]).arg(conv.scale()).arg(conv.offset());
            _odf->setKeyValue("BaseMap","Range",rangeString);
            _odf->setKeyValue("BaseMap","Domain","value.dom");

            _odf->setKeyValue("BaseMap","MinMax",QString("%1:%2").arg(stats[NumericStatistics::pMIN]).arg(stats[NumericStatistics::pMAX]));
            QString domInfo = QString("value.dom;Long;value;0;-9999999.9:9999999.9:0.1:offset=0");
            _odf->setKeyValue("BaseMap","DomainInfo",domInfo);
        }
    } if ( dom->ilwisType() == itITEMDOMAIN) {
        QString source = Resource::toLocalFile(dom->source().url(), true);
        if ( dom->valueType() == itTHEMATICITEM && coverage->ilwisType() == itRASTER) {
            IThematicDomain themdom = dom.get<ThematicDomain>();
            if ( themdom.isValid()) {
                QString domInfo = QString("%1;Byte;class;%2;;").arg(source).arg(themdom->count());
                _odf->setKeyValue("BaseMap","DomainInfo",domInfo);
                _odf->setKeyValue("BaseMap","Domain",source);
            }
        } else if(dom->valueType() == itINDEXEDITEM) {
            QString domName = _odf->file();
            QString domInfo = QString("%1;Long;UniqueID;0;;").arg(domName);
            _odf->setKeyValue("BaseMap","DomainInfo",domInfo);
            _odf->setKeyValue("BaseMap","Domain",domName);
        } else if ( dom->valueType() == itNAMEDITEM) {
            INamedIdDomain iddom = dom.get<NamedIdDomain>();
            QString domName = _odf->file();
            int index;
            if ( (index=domName.lastIndexOf("."))!= -1)             {
                domName = domName.left(index);
            }
            QString domInfo = QString("%1;;Int;id;%2;;").arg(domName).arg(iddom->count());
            _odf->setKeyValue("BaseMap","DomainInfo",domInfo);
            _odf->setKeyValue("BaseMap","Domain",domName);
            iddom->connectTo(QUrl(),"domain","ilwis3", IlwisObject::cmOUTPUT);
            iddom->store(Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA);
        }
    }

    ITable attTable = coverage->attributeTable();
    if ( attTable.isValid()) {
        QScopedPointer<TableConnector> conn(createTableConnector(attTable, coverage, type));
        conn->storeMetaData(attTable.ptr());
    }
    return true;
}

bool CoverageConnector::storeBinaryData(IlwisObject *obj, IlwisTypes tp)
{
    Coverage *coverage = static_cast<Coverage *>(obj);
    ITable attTable = coverage->attributeTable();
    if ( attTable.isValid()) {
        QScopedPointer<TableConnector> conn(createTableConnector(attTable, coverage, tp));
        return conn->storeBinaryData(attTable.ptr());

    }

    return false;
}

TableConnector *CoverageConnector::createTableConnector(ITable& attTable, Coverage *coverage, IlwisTypes tp) {
    QString dataFile = coverage->name();
    QString attDom = dataFile;
    if ( hasType(tp,itRASTER)) {
        RasterCoverage *raster = static_cast<RasterCoverage *>(coverage);
        Resource resource = raster->datadef().domain()->source();
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
    _odf->setKeyValue("BaseMap","AttributeTable",dataFile + ".tbt");
    attTable->setName(dataFile);
    QString dir = context()->workingCatalog()->location().toLocalFile();
    QString filename = dir + "/" + dataFile + ".tbt";
    TableConnector *conn = new TableConnector(Resource(QUrl::fromLocalFile(filename), itTABLE), false);
    //attribute domains are comming from ilwis4 always uniqueid and based on the map
    conn->attributeDomain(attDom);

    return conn;
}
