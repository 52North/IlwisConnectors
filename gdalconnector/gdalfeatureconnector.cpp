#include <QSqlQuery>
#include <QSqlError>
#include <fstream>
#include <iterator>

#include "kernel.h"
#include "coverage.h"
#include "module.h"
#include "coverage.h"
#include "polygon.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "table.h"
#include "geometry.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "textdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"
#include "coverageconnector.h"
#include "juliantime.h"

#include "gdalfeatureconnector.h"

using namespace Ilwis;
using namespace Gdal;

GdalFeatureConnector::GdalFeatureConnector(const Resource &resource, bool load) : CoverageConnector(resource,load){
}

ConnectorInterface* GdalFeatureConnector::create(const Resource &resource, bool load) {
    return new GdalFeatureConnector(resource, load);
}

Ilwis::IlwisObject* GdalFeatureConnector::create() const{
    return new FeatureCoverage(this->_resource);
}

IlwisTypes GdalFeatureConnector::translateOGRType(OGRwkbGeometryType type) const{
    IlwisTypes ret = itUNKNOWN;

    if ( type == wkbPoint || type == wkbMultiPoint || type == wkbPoint25D || type == wkbMultiPoint25D)
        ret += itPOINT;

    if ( type == wkbLineString || type == wkbMultiLineString || type == wkbLineString25D || type == wkbMultiLineString25D)
        ret += itLINE;

    if ( type == wkbPolygon || type == wkbMultiPolygon || type == wkbPolygon25D || type == wkbMultiPolygon25D)
        ret += itPOLYGON;

    if ( type == wkbGeometryCollection )
        ret = itFEATURE;

    return ret;
}


bool GdalFeatureConnector::loadMetaData(Ilwis::IlwisObject *data){

    if(!CoverageConnector::loadMetaData(data))
        return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IlwisTypes coverageType = itUNKNOWN;
    int featureCount = 0;
    Box2D<double> bbox;
    bool initMinMax = 0;

    ITable attTable;
    Resource resource(QUrl(QString("ilwis://internal/%1").arg(_filename)), itFLATTABLE);
    if(!attTable.prepare(resource)) {//only internalTableconnector is used! own class not needed
        ERROR1(ERR_NO_INITIALIZED_1,resource.name());
        return false;
    }
    IDomain dmKey;
    dmKey.prepare("count");
    ColumnDefinition colKey(FEATUREIDCOLUMN, dmKey, 0);
    attTable->addColumn(colKey);
    ColumnDefinition colCovKey(COVERAGEKEYCOLUMN, dmKey, 1);
    attTable->addColumn(colCovKey);

    for(int layer = 0; layer < gdal()->getLayerCount(_handle->handle()) ; ++layer) {
        OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), layer);
        if ( hLayer) {

            //TODO is this to be done in gdalFeatureTableConnector!?!
            OGRFeatureDefnH hLayerDef = gdal()->getLayerDef(hLayer);
            int fieldCount = gdal()->getFieldCount(hLayerDef);
            for (int i = 0; i < fieldCount; i++){
                OGRFieldDefnH hFieldDefn = gdal()->getFieldDfn(hLayerDef, i);
                QString name = QString(gdal()->getFieldName(hFieldDefn));
                OGRFieldType type = gdal()->getFieldType(hFieldDefn);
                //OGR_Fld_GetWidth, OGR_Fld_GetPrecision, OGR_Fld_IsIgnored
                IDomain domain;
                switch(type){
                    case OFTInteger:{
                         domain = INumericDomain();
                         domain.prepare("integer"); break;// Simple 32bit integer
                    }
                    case OFTIntegerList:break; // List of 32bit integers

                    case OFTReal:{
                        domain = INumericDomain();
                        domain.prepare("value"); break; // Double Precision floating point
                    }
                    case OFTRealList: // List of doubles
                    case OFTString:{ // String of ASCII chars
                        ITextDomain tdomain;
                        tdomain.prepare();
                        domain = tdomain; break;
                    }
                    case OFTStringList: // Array of strings
                    case OFTWideString:// deprecated
                    case OFTWideStringList:// deprecated
                    case OFTBinary: break; // Raw Binary data

                    case OFTDate:  // Date
                    case OFTTime:  // itTime - itTIMEDOMAIN
                    case OFTDateTime:{  // Date and Time
                        INumericDomain ndomain;
                        ndomain.prepare("value");
                        ndomain->setRange(new TimeInterval());//TODO: ilwisType of Domain should change to itTIMEDOMAIN
                        domain = ndomain;  break;
                    }
                }

                ColumnDefinition colDef(name, domain,i+2);
                attTable->addColumn(colDef);
            }

            //feature types
            IlwisTypes type = translateOGRType(gdal()->getLayerGeometry(hLayer));
            if (type == itUNKNOWN){
                ERROR2(ERR_COULD_NOT_LOAD_2,QString("layer from: %1").arg(_filename),QString(":%1").arg(gdal()->getLastErrorMsg()));
            }
            coverageType |= type;

            //feature counts
            int temp = gdal()->getFeatureCount(hLayer, FALSE);//TRUE to FORCE databases to scan whole layer, FALSe can end up in -1 for unknown result
            featureCount = fcoverage->featureCount(type);
            featureCount += (temp == -1) ? 0 : temp;
            fcoverage->setFeatureCount(type, featureCount);

            //layer envelopes/extents
            OGREnvelope envelope;//might sometimes be supported as 3D now only posssible from OGRGeometry
            OGRErr er = gdal()->getLayerExtent(hLayer, &envelope , FALSE);//TRUE to FORCE
            if (er ==  OGRERR_FAILURE){
                ERROR2(ERR_COULD_NOT_LOAD_2,QString("(TRY FORCE) extent of a layer from: %2").arg(_filename),QString(":%1").arg(gdal()->getLastErrorMsg()));
            }
            if(!initMinMax){
                bbox=Box2D<double>(Coordinate2d(envelope.MinX,envelope.MinY),Coordinate2d(envelope.MaxX,envelope.MaxY));
            }else{
                if(bbox.max_corner().x() < envelope.MaxX)
                    bbox.max_corner().x(envelope.MaxX);
                if(bbox.max_corner().y() < envelope.MaxY)
                    bbox.max_corner().y(envelope.MaxY);
                if(bbox.min_corner().x() > envelope.MinX)
                    bbox.min_corner().x(envelope.MinX);
                if(bbox.min_corner().y() > envelope.MinY)
                    bbox.min_corner().y(envelope.MinY);
            }
        }
    }
    fcoverage->attributeTable(attTable);
    if (coverageType != itUNKNOWN && featureCount >= 0){
        fcoverage->featureTypes(coverageType);
        fcoverage->envelope(bbox);
    }else{
       return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Records",data->name());
    }

//    ITable tbl = fcoverage->attributeTable();
//    tbl->setRows(fcoverage->featureCount());

    return true;
}

bool GdalFeatureConnector::loadBinaryData(IlwisObject* data){
    if ( data == nullptr)
        return false;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    if ( fcoverage->isValid() ) {
        for(int layer = 0; layer < gdal()->getLayerCount(_handle->handle()) ; ++layer) {
            OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), layer);
            if ( hLayer) {
                OGRFeatureDefnH hFeatureDef = gdal()->getLayerDef(hLayer);           
                if ( hFeatureDef) {
                    quint64 rec = 1;
                    OGRFeatureH hFeature;
                    gdal()->resetReading(hLayer);
                    while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL){
                        std::vector<SPFeatureI> features = fillFeature(fcoverage, gdal()->getGeometryRef(hFeature));

                        if (!features.empty()){
                            ITable attTable = fcoverage->attributeTable();
                            if (!attTable.isValid())
                            return false;
                            std::vector<QVariant> record;
                            record.push_back(QVariant((*features.begin())->featureid()));//FEATUREIDCOLUMN
                            record.push_back(QVariant(rec++));//COVERAGEKEYCOLUMN
                            for (int i = 2; i < attTable->columns();i++){
                                ColumnDefinition coldef = attTable->columndefinition(i);
                                if(coldef.datadef().domain().isValid()){
                                    switch(coldef.datadef().domain()->ilwisType()){
                                        case itNUMERICDOMAIN:{
                                            record.push_back(QVariant(gdal()->getFieldAsDouble(hFeature, i-2)));
                                            break;
                                        }
                                        case itTIMEDOMAIN:{
                                            Time time;
                                            int year,month,day,hour,minute,second,TZFlag;
                                            if (gdal()->getFieldAsDateTime(hFeature,i-2,&year,&month,&day,&hour,&minute,&second,&TZFlag)){
                                                time.setDay(day);
                                                time.setHour(hour);
                                                time.setMinute(minute);
                                                time.setMonth(month);
                                                time.setSecond(second);
                                                time.setYear(year);
                                            }
                                            record.push_back(QVariant((double)time));
                                            break;
                                        }
                                        case itTEXTDOMAIN: {
                                            record.push_back(QVariant(gdal()->getFieldAsString(hFeature, i-2)));
                                            break;
                                        }
                                        default: record.push_back(QVariant());
                                    }
                                }else{
                                    record.push_back(QVariant());
                                }

                            }
                            for(const SPFeatureI& feat : features) {
                                record[0] = QVariant(feat->featureid());
                                attTable->record(NEW_RECORD,record);
                            }
                        }else{
                            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, QString("Record: %1").arg(rec), _filename);
                        }

                        gdal()->destroyFeature( hFeature );
                    }
                }
            }
        }
    }
    return true;
}

std::vector<SPFeatureI> GdalFeatureConnector::fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry) const{
    std::vector<SPFeatureI> ret;
    if (geometry){
        OGRwkbGeometryType type = gdal()->getGeometryType(geometry);
        switch (translateOGRType(type)){
            case itUNKNOWN: ERROR2(ERR_INVALID_PROPERTY_FOR_2, "GeometryType of a Feature", _filename); return ret;
            case itPOINT:   return fillPoint(fcoverage, geometry);    break;
            case itLINE:    return fillLine(fcoverage, geometry);     break;
            case itPOLYGON: return fillPolygon(fcoverage, geometry, type);  break;
            default:{ //possibly wbkGeometryCollection or other kind of mixture
                int subGeomCount = gdal()->getSubGeometryCount(geometry);
                if(subGeomCount){
                    for(int i = 0; i < subGeomCount; ++i) {
                        OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
                        if(hSubGeometry){
                            std::vector<SPFeatureI> tmp =  fillFeature(fcoverage, hSubGeometry);
                            ret.insert(ret.end(), tmp.begin(), tmp.end());
                        }
                    }
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}
std::vector<SPFeatureI> GdalFeatureConnector::fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);
    std::vector<SPFeatureI> ret;
    if(subGeomCount > 0){
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                std::vector<SPFeatureI> tmp =  fillPoint(fcoverage, hSubGeometry);
                ret.insert(ret.end(), tmp.begin(), tmp.end());
            }
        }
        return ret;
    }else{
        double x,y,z;
        gdal()->getPoints(geometry, 0,&x,&y,&z);
        Coordinate coord(x,y,z);
        Geometry point(coord);
        ret.push_back(fcoverage->newFeature({point}));
        return ret;
    }
}
std::vector<SPFeatureI> GdalFeatureConnector::fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);
    std::vector<SPFeatureI> ret;
    if(subGeomCount > 0){
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                std::vector<SPFeatureI> tmp =  fillLine(fcoverage, hSubGeometry);
                ret.insert(ret.end(), tmp.begin(), tmp.end());
            }
        }
        return ret;
    }else{
        int count = gdal()->getPointCount(geometry);
         if ( count == 0)
             return ret;
         ITable attTable = fcoverage->attributeTable();
         if (!attTable.isValid())
             return ret;

         Line2D<Coordinate2d > line;
         line.resize(count);
         for(int i = 0; i < count; ++i) {
             double x,y,z;
             gdal()->getPoints(geometry, i,&x,&y,&z);
             line[i] = Coordinate2d(x,y);
//             attTable->cell(FEATUREVALUECOLUMN, rec, QVariant(z));
         }
        ret.push_back(fcoverage->newFeature({line}));
        return ret;
    }
}

std::vector<SPFeatureI> GdalFeatureConnector::fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry, OGRwkbGeometryType type) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);//error!check type!
    std::vector<SPFeatureI> ret;
    if ( type == wkbPolygon || type == wkbPolygon25D ){
        OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, 0);
        if(hSubGeometry){
            int count = gdal()->getPointCount(hSubGeometry);
            if ( count == 0)
                return ret;
            ITable attTable = fcoverage->attributeTable();
            if (!attTable.isValid())
                return ret;

            Polygon pol;
            std::vector<Coordinate2d>& ring = pol.outer();
            ring.resize(count);

            for(int i = 0; i < count; ++i) {
                double x,y,z;
                gdal()->getPoints(hSubGeometry, i,&x,&y,&z);
                ring[i] = Coordinate2d(x,y);
//                tbl->cell(FEATUREVALUECOLUMN, i, QVariant(z));
            }

            pol.inners().resize(subGeomCount-1);
            for(int j = 1; j < subGeomCount; ++j) {
                hSubGeometry = gdal()->getSubGeometryRef(geometry, j);
                if(hSubGeometry){
                    count = gdal()->getPointCount(hSubGeometry);
                    if(count == 0)
                       continue;
                    ring = pol.inners()[j];
                    ring.resize(count);
                    for(int i = 0; i < count; ++i) {
                        double x,y,z;
                        gdal()->getPoints(hSubGeometry, i,&x,&y,&z);
                        ring[i] = Coordinate2d(x,y);
//                        tbl->cell(FEATUREVALUECOLUMN, i, QVariant(z));
                    }
                }
            }
            ret.push_back(fcoverage->newFeature({pol}));
            return ret;
        }else{
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"polygon for a record",_filename);
            return ret;
        }
    }else if (type == wkbMultiPolygon || type == wkbMultiPolygon25D){
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                std::vector<SPFeatureI> tmp =  fillPolygon(fcoverage, hSubGeometry, wkbPolygon);
                ret.insert(ret.end(), tmp.begin(), tmp.end());
            }
        }
        return ret;
    }else{
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"polygon for a record",_filename);
        return ret;
    }
}

bool GdalFeatureConnector::store(IlwisObject *obj, IlwisTypes type)
{
    return CoverageConnector::store(obj, type);
}
