#include <QSqlQuery>
#include <QSqlError>
#include <QRegExp>

#include "kernel.h"
#include "coverage.h"
#include "module.h"
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

CoverageConnector::CoverageConnector(const Resource &item, bool load) : Ilwis3Connector(item, load)
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

bool CoverageConnector::loadMetaData(Ilwis::IlwisObject *data)
{
    Ilwis3Connector::loadMetaData(data);

    Coverage *coverage = static_cast<Coverage *>(data);
    QString csyName = _odf->value("BaseMap","CoordSystem");
    if ( csyName.toLower() == "latlonwgs84.csy")
        csyName = "code=epsg:4326";
    ICoordinateSystem csy;
    if ( !csy.prepare(csyName)) {
        kernel()->issues()->log(csyName,TR("Coordinate system couldnt be initialized, defaulting to 'unknown'"),IssueObject::itWarning);
        QString res = QString("ilwis://file/unknown.csy");
        if (!csy.prepare(res)) {
            kernel()->issues()->log(TR("Fallback to 'unknown failed', corrupt system files defintion"));
            return false;
        }
    }
    coverage->setCoordinateSystem(csy);

    QString file = _odf->value("BaseMap", "AttributeTable");
    ITable attTable;
    if ( file != sUNDEF) {
        if(!attTable.prepare(file))
            kernel()->issues()->log(data->name(),TR(ERR_NO_INITIALIZED_1).arg(data->name()),IssueObject::itWarning);
    }
    if (!attTable.isValid() && coverage->ilwisType() != itGRID)   {
        Resource res(QUrl(QString("ilwis://internal/%1").arg(_odf->fileinfo().baseName())), itFLATTABLE);
        if(!attTable.prepare(res))
            return ERROR1(ERR_NO_INITIALIZED_1,data->name());

    }
    coverage->attributeTable(ilwisType(_odf->fileinfo().fileName()),attTable);
    IDomain dom;
    if(!dom.prepare(_odf->fileinfo().canonicalFilePath())) {
        kernel()->issues()->log(data->name(),TR(ERR_NO_INITIALIZED_1).arg(data->name()));
        return false;
    }

    coverage->datadef() = DataDefinition(dom);
    double vmax,vmin,scale,offset;
    QString range = _odf->value("BaseMap","Range");
    if ( range != sUNDEF ) {
        if ( getRawInfo(range, vmin,vmax,scale,offset)) {
            if ( scale == 1.0) {
                coverage->datadef().range(new NumericRange(vmin, vmax,1));

            }
            else {
                coverage->datadef().range(new NumericRange(vmin, vmax));
            }


        }
    } else {
        QString dminfo = _odf->value("BaseMap","DomainInfo");
        if ( dminfo != sUNDEF) {
            int index = dminfo.indexOf("class;");
            if ( index != -1) {
                _converter = RawConverter("class");
            } else {
                index = dminfo.indexOf("id;");
                if ( index != -1) {
                    _converter = RawConverter("id");
                } else {
                    index = dminfo.indexOf("UniqueID;");
                    if ( index != -1) {
                        _converter = RawConverter("UniqueID");
                    }
                }
            }

        }
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

bool CoverageConnector::storeMetaData(IlwisObject *obj)
{
    bool ok = Ilwis3Connector::storeMetaData(obj);
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

    const IDomain dom = coverage->datadef().domain();
    if (!dom.isValid())
        return ERROR2(ERR_NO_INITIALIZED_2, "Domain", coverage->name());

    calcStatics(obj,CoverageStatistics::pBASIC);
    if ( dom->ilwisType() == itNUMERICDOMAIN) {

        quint16 digits = coverage->statistics().significantDigits();
        qint32 delta = coverage->statistics().max() - coverage->statistics().min();
        if ( delta >= 0 && delta < 256 && digits == 0){
            if ( delta >= 0 && delta < 256 && digits == 0){
                if ( coverage->datadef().domain()->code() == "boolean"){
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
            RawConverter conv(coverage->statistics().min(), coverage->statistics().max(),pow(10, -digits))                ;
            QString rangeString = QString("%1:%2:%3:offset=%4").arg(coverage->statistics().min()).arg(coverage->statistics().max()).arg(conv.scale()).arg(conv.offset());
            _odf->setKeyValue("BaseMap","Range",rangeString);
            _odf->setKeyValue("BaseMap","Domain","value.dom");

            _odf->setKeyValue("BaseMap","MinMax",QString("%1:%2").arg(coverage->statistics().min()).arg(coverage->statistics().max()));
            QString domInfo = QString("value.dom;Long;value;0;-9999999.9:9999999.9:0.1:offset=0");
            _odf->setKeyValue("BaseMap","DomainInfo",domInfo);
        }
    } if ( dom->ilwisType() == itITEMDOMAIN) {
        QString source = Resource::toLocalFile(dom->source().url(), true);
        if ( dom->valueType() == itTHEMATICITEM) {
            IThematicDomain themdom = dom.get<ThematicDomain>();
            if ( themdom.isValid()) {
                QString domInfo = QString("%1;Byte;class;%2;;").arg(source).arg(themdom->count());
                _odf->setKeyValue("BaseMap","DomainInfo",domInfo);
                _odf->setKeyValue("BaseMap","Domain",source);
            }
        } else if ( dom->valueType() == itINDEXEDITEM) {
            _odf->setKeyValue("BaseMap","Domain","UniqueID");
        }
    }

    ITable attTable = coverage->attributeTable(itPOLYGON);
    if ( attTable.isValid()) {
        QString dataFile = coverage->name();
        int index = dataFile.lastIndexOf(".");
        if ( index != -1) {
            dataFile = dataFile.left(index);
        }
        _odf->setKeyValue("BaseMap","AttributeTable",dataFile + ".tbt");
        QString dir = context()->workingCatalog()->location().toLocalFile();
        QString filename = dir + "/" + dataFile + ".tbt";
        TableConnector conn(Resource(QUrl::fromLocalFile(filename), itTABLE), false);
        conn.storeMetaData(attTable.ptr());
    }
    return true;
}

