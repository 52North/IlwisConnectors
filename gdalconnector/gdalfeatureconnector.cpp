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
#include "featureiterator.h"
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
    Resource resource(QUrl(QString("ilwis://internalcatalog/%1_%2").arg(fcoverage->name()).arg(fcoverage->id())), itFLATTABLE);
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

//    for(int layer = 0; layer < gdal()->getLayerCount(_handle->handle()) ; ++layer) {
        int layer = 0;//only first layer will be read/fit into single FeatureCoverage(*data)
        OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), layer);
        if ( hLayer) {

            OGRFeatureDefnH hLayerDef = gdal()->getLayerDef(hLayer);
            int fieldCount = gdal()->getFieldCount(hLayerDef);
            for (int i = 0; i < fieldCount; i++){
                OGRFieldDefnH hFieldDefn = gdal()->getFieldDfn(hLayerDef, i);
                QString name = QString(gdal()->getFieldName(hFieldDefn));
                OGRFieldType type = gdal()->getFieldType(hFieldDefn);
                //OGR_Fld_GetWidth, OGR_Fld_GetPrecision, OGR_Fld_IsIgnored
                IDomain domain;
                switch(type){
                    case OFTInteger:{ // Simple 32bit integer
                         INumericDomain ndomain;
                         ndomain.prepare("integer");
                         domain = ndomain;break;
                    }
                    case OFTIntegerList:break; // List of 32bit integers

                    case OFTReal:{ // Double Precision floating point
                        INumericDomain ndomain;
                        ndomain.prepare("value");
                        domain = ndomain; break;
                    }
                    case OFTRealList:break; // List of doubles
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
                        ndomain->range(new TimeInterval());//TODO: ilwisType of Domain should change to itTIMEDOMAIN
                        domain = ndomain;  break;
                    }
                }

                ColumnDefinition colDef(name, domain,i+2);
                attTable->addColumn(colDef);
            }

            //feature types
            IlwisTypes type = translateOGRType(gdal()->getLayerGeometry(hLayer));
            if (type == itUNKNOWN){
                ERROR2(ERR_COULD_NOT_LOAD_2,QString("layer from: %1").arg(_filename.toString()),QString(":%1").arg(gdal()->getLastErrorMsg()));
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
                ERROR2(ERR_COULD_NOT_LOAD_2,QString("(TRY FORCE) extent of a layer from: %2").arg(_filename.toString()),QString(":%1").arg(gdal()->getLastErrorMsg()));
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
//    }
    fcoverage->attributeTable(attTable);
    if (coverageType != itUNKNOWN && featureCount >= 0){
        fcoverage->featureTypes(coverageType);
        fcoverage->envelope(bbox);
        fcoverage->coordinateSystem()->envelope(bbox);
    }else{
       return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Records",data->name());
    }

//    ITable tbl = fcoverage->attributeTable();
//    tbl->setRows(fcoverage->featureCount());

    return true;
}

QVariant GdalFeatureConnector::fillEmptyColumn(OGRFeatureH , int , SPRange ){
    return QVariant();
}

QVariant GdalFeatureConnector::fillStringColumn(OGRFeatureH featureH, int colIntex, SPRange range){
    return QVariant(gdal()->getFieldAsString(featureH, colIntex));
}

QVariant GdalFeatureConnector::fillIntegerColumn(OGRFeatureH featureH, int colIntex, SPRange range){
    int v = gdal()->getFieldAsInt(featureH, colIntex);
    SPNumericRange nrange = range.staticCast<NumericRange>();
    if (nrange)
        (*nrange) += (double)v;
    return QVariant(v);
}
//TODO: not yet tested
QVariant GdalFeatureConnector::fillDoubleColumn(OGRFeatureH featureH, int colIntex, SPRange range){
    double v = gdal()->getFieldAsDouble(featureH, colIntex);
    SPNumericRange nrange = range.staticCast<NumericRange>();
    if (nrange)
        (*nrange) += v;
    return QVariant(v);
}
//TODO: not yet tested
QVariant GdalFeatureConnector::fillDateTimeColumn(OGRFeatureH featureH, int colIntex, SPRange range){
    Time time;
    double v = rUNDEF;
    int year,month,day,hour,minute,second,TZFlag;
    if (gdal()->getFieldAsDateTime(featureH,colIntex,&year,&month,&day,&hour,&minute,&second,&TZFlag)){
        time.setDay(day);
        time.setHour(hour);
        time.setMinute(minute);
        time.setMonth(month);
        time.setSecond(second);
        time.setYear(year);
        v = time;
    }

    SPNumericRange nrange = range.staticCast<NumericRange>();
    if (nrange)
        (*nrange) += v;
    return QVariant(v);
}

bool GdalFeatureConnector::loadBinaryData(IlwisObject* data){
    if ( data == nullptr)
        return false;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    if ( fcoverage->isValid() ) {
        ITable attTable = fcoverage->attributeTable();
        if (!attTable.isValid()){
            ERROR2(ERR_NO_INITIALIZED_2,"attribute table",_filename.toString());
            return false;
        }
        std::vector<FillerColumnDef*> columnDefinitions;
        columnDefinitions.resize(attTable->columnCount());
        fcoverage->setFeatureCount(itPOLYGON, 0); // metadata already set it to correct number, creating new features will up the count agains; so reset to 0.
        fcoverage->setFeatureCount(itLINE, 0);
        fcoverage->setFeatureCount(itPOINT, 0);

        //each LAYER
//      for(int layer = 0; layer < gdal()->getLayerCount(_handle->handle()) ; ++layer) {
            int layer = 0;//only the first layer fits into one FeatureCoverage
            OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), layer);
            if ( hLayer) {
                //check again if attTable ColumnDefinitions match still gdalFieldDefinition (additional or removed columns are detected by names
                OGRFeatureDefnH hLayerDef = gdal()->getLayerDef(hLayer);
                int gdalFieldCount = gdal()->getFieldCount(hLayerDef);
                int offset = -2;
                for (int i = 2; i < attTable->columnCount();i++){
                    if (i+offset < gdalFieldCount){//check if coulumn was added to metadata
                        OGRFieldDefnH hFieldDefn = gdal()->getFieldDfn(hLayerDef, i+offset);
                        QString name = QString(gdal()->getFieldName(hFieldDefn));
                        ColumnDefinition coldef = attTable->columndefinition(i);
                        if (coldef.name().compare(name,Qt::CaseSensitive) == 0){//check if column was deleted from metadata
                            DataDefinition datadef = coldef.datadef();
                            if(datadef.domain().isValid()){
                                IlwisTypes tp = datadef.domain()->valueType();
                                if (tp & itSTRING){
                                    columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillStringColumn, i+offset, datadef.range());
                                }else if (tp & itINTEGER){
                                    NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
                                    //creating the actual range as invalid to be adjusted in the fillers
                                    double min = r->min();
                                    r->min(r->max());
                                    r->max(min);
                                    datadef.range(r);
                                    columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillIntegerColumn, i+offset, datadef.range());
                                }else if (tp & itDOUBLE){
                                    //creating the actual range as invalid to be adjusted in the fillers
                                    NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
                                    double min = r->min();
                                    r->min(r->max());
                                    r->max(min);
                                    datadef.range(r);
                                    columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillDoubleColumn, i+offset, datadef.range());
                                }else if (tp & itTIME){
                                    //creating the actual range as invalid to be adjusted in the fillers
                                    NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
                                    double min = r->min();
                                    r->min(r->max());
                                    r->max(min);
                                    datadef.range(r);
                                    columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillDateTimeColumn, i+offset, datadef.range());
                                }else{
                                    columnDefinitions[i] = nullptr;
                                }
                            }else{
                                columnDefinitions[i] = nullptr;
                            }
                        }else{//column was deleted from metadata
                            columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillEmptyColumn, -1, SPRange(NULL));
                            offset++;
                        }
                    }else{//coulumn was added to metadata
                        columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillEmptyColumn, -1, SPRange(NULL));
                    }
                }

                std::vector<QVariant> record;
                record.resize(attTable->columnCount());


                quint64 rec = 0;
                OGRFeatureH hFeature;
                gdal()->resetReading(hLayer);
                //each FEATURE
                while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL){
                    //create ilwisFeatures from Geometry - multiple on wkbMultiPolygon/-Line/-Point and wkbGeometryCollection
                    std::vector<SPFeatureI> features = fillFeature(fcoverage, gdal()->getGeometryRef(hFeature));
                    if (!features.empty()){
                        record[1] = QVariant(++rec);//COVERAGEKEYCOLUMN
                        //each OGR_F_FIELD > RECORD
                        for (int i = 2; i < attTable->columnCount();i++){
                            if (columnDefinitions[i]){
                                record[i] = (this->*columnDefinitions[i]->fillFunc)(hFeature, columnDefinitions[i]->index, columnDefinitions[i]->range);
                            }
                        }
                        for(const SPFeatureI& feat : features) {
                            record[0] = QVariant(feat->featureid());
                            attTable->record(attTable->recordCount()-1,record);//should overwrite the last record in attTable, which hopefully is the one created by fcoverage->newFeature({geometry}) within FillFeature
                        }
                    }else{
                        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, QString("Record: %1").arg(rec), _filename.toString());
                    }

                    gdal()->destroyFeature( hFeature );
                }

            }
//      }
    }
    return true;
}

std::vector<SPFeatureI> GdalFeatureConnector::fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry) const{
    std::vector<SPFeatureI> ret;
    if (geometry){
        OGRwkbGeometryType type = gdal()->getGeometryType(geometry);
        switch (translateOGRType(type)){
            case itUNKNOWN: ERROR2(ERR_INVALID_PROPERTY_FOR_2, "GeometryType of a Feature", _filename.toString()); return ret;
            case itPOINT:   return fillPoint(fcoverage, geometry);    break;
            case itLINE:    return fillLine(fcoverage, geometry);     break;
            case itPOLYGON: return fillPolygon(fcoverage, geometry, type);  break;
            default:{ //possibly wkbGeometryCollection or other kind of mixture
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
        Geometry point(coord, fcoverage->coordinateSystem());
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
//             attTable->setCell(FEATUREVALUECOLUMN, rec, QVariant(z));
         }
        ret.push_back(fcoverage->newFeature({line, fcoverage->coordinateSystem()}));
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
//                tbl->setCell(FEATUREVALUECOLUMN, i, QVariant(z));
            }

            pol.inners().resize(subGeomCount-1);
            for(int j = 1; j < subGeomCount; ++j) {
                hSubGeometry = gdal()->getSubGeometryRef(geometry, j);
                if(hSubGeometry){
                    count = gdal()->getPointCount(hSubGeometry);
                    if(count == 0)
                       continue;
                    ring = pol.inners()[j-1];
                    ring.resize(count);
                    for(int i = 0; i < count; ++i) {
                        double x,y,z;
                        gdal()->getPoints(hSubGeometry, i,&x,&y,&z);
                        ring[i] = Coordinate2d(x,y);
//                        tbl->setCell(FEATUREVALUECOLUMN, i, QVariant(z));
                    }
                }
            }
            ret.push_back(fcoverage->newFeature({pol, fcoverage->coordinateSystem()}));
            return ret;
        }else{
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"polygon for a record",_filename.toString());
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
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"polygon for a record",_filename.toString());
        return ret;
    }
}


OGRDataSourceH GdalFeatureConnector::createFileBasedDataSource(const QString& postfix, const QFileInfo& fileinfo) const {
    QString outputname = fileinfo.absolutePath() + "/" + fileinfo.baseName() + postfix + "." + fileinfo.suffix();
    if ( QFileInfo(outputname).exists()){
        if ( remove(outputname.toLocal8Bit())!= 0){
            //if(!QFile::remove(outputname)) {
                ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1,outputname);
                //return 0;
            }
    }
    OGRDataSourceH datasource = gdal()->createDatasource(_driver, outputname.toLocal8Bit(),0);
    if ( datasource == 0) {
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "data source",fileinfo.fileName());
        return 0;
    }

    return datasource;
}

OGRLayerH GdalFeatureConnector::createLayer(const QString& name,OGRwkbGeometryType type,OGRSpatialReferenceH srs, OGRDataSourceH  source ) {
    if ( source == 0)
        return 0;
    OGRLayerH layer = gdal()->createOgrLayer(source, name.toLocal8Bit(),srs,type, 0);
    if ( layer == 0){
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "layer",name);
        return 0;
    }
    return layer;
}

bool GdalFeatureConnector::createAttributes(const ITable& tbl, OGRLayerH layer, const std::vector<OGRFieldDefnH>& fielddefs,std::vector<bool>& validAttributes) {
    if ( layer == 0)
        return false;

    int index=0;
    for(int i=0; i < tbl->columnCount(); ++i){
        if ( validAttributes[i]) {
            if(gdal()->addAttribute(layer,fielddefs[index],TRUE) != OGRERR_NONE){
                validAttributes[i] = false;
                WARN2(ERR_NO_INITIALIZED_2,tbl->columndefinition(i).name(),tbl->name());
            }
            ++index;
        }
    }
    return true;
}

OGRwkbGeometryType GdalFeatureConnector::ilwisType2GdalFeatureType(IlwisTypes tp) {
    switch (tp) {
    case itPOINT:
        return wkbPoint;
    case itLINE:
        return wkbLineString;
    case itPOLYGON:
        return wkbPolygon;
    }
    return wkbUnknown;
}

void GdalFeatureConnector::setAttributes(OGRFeatureH hfeature, SPFeatureI feature, const std::vector<bool>& validAttributes) {
    int index = 0;
    for(int i=0; i < feature->attributeColumnCount(); ++i){
        const ColumnDefinition& coldef  = feature->columndefinition(i);
        if ( !validAttributes[i])
            continue;

        if(hasType(coldef.datadef().domain()->valueType(),itINTEGER)) {
            qint32 val = feature->cell(coldef.name()).toInt();
            gdal()->setIntegerAttribute(hfeature,index,val);
        } else if (hasType(coldef.datadef().domain()->valueType(),itDOUBLE | itFLOAT)) {
            double val = feature->cell(coldef.name()).toDouble();
            gdal()->setDoubleAttribute(hfeature,index,val);
        } else if (hasType(coldef.datadef().domain()->valueType(),itDOMAINITEM)) {
            QString val = feature->cell(coldef.name(),-1, false).toString();
            gdal()->setStringAttribute(hfeature,index,val.toLocal8Bit());
        }
        //TODO coords, time ..
        ++index;
    }

}

bool GdalFeatureConnector::setDataSourceAndLayers(const IFeatureCoverage& features, std::vector<SourceHandles>& datasources,std::vector<bool>& validAttributes) {

    QFileInfo fileinfo = containerConnector(IlwisObject::cmOUTPUT)->toLocalFile(_filename);
    AttributeTable tbl = features->attributeTable();
    validAttributes.resize(tbl->columnCount(), false);
    std::vector<OGRFieldDefnH> fielddefs(tbl->columnCount() - 1); // no feature_id in external table

    int index = 0;
    for(int i=0; i < tbl->columnCount(); ++i){
        if ( tbl->columndefinition(i).name() == FEATUREIDCOLUMN ){
            continue;
        }
        OGRFieldType ogrtype = ilwisType2GdalFieldType(tbl->columndefinition(i).datadef().domain()->valueType());
        OGRFieldDefnH fieldef = gdal()->createAttributeDefintion(tbl->columndefinition(i).name().toLocal8Bit(),ogrtype);
        if ( fieldef == 0){
            WARN2(ERR_INVALID_INIT_FOR_2, TR("data-type"), tbl->columndefinition(i).name());
        }else
            validAttributes[i] = true;

        fielddefs[index++] = fieldef;
    }

    OGRSpatialReferenceH srs = createSRS(features->coordinateSystem());
    IlwisTypes types = features->featureTypes();
    bool multipleoutputs = (types == (itPOINT | itLINE)) || (types == (itPOINT | itPOLYGON)) || (types == (itLINE | itPOLYGON));
    if ( multipleoutputs){
        if ((features->featureTypes() & itPOINT) != 0) {
            datasources[ilwisType2Index(itPOINT)]._source =  createFileBasedDataSource("_poinst", fileinfo);
            datasources[ilwisType2Index(itPOINT)]._layers[0] = createLayer(features->name() + "_points", ilwisType2GdalFeatureType(itPOINT), srs,datasources[0]._source);
            if(!createAttributes(tbl, datasources[ilwisType2Index(itPOINT)]._layers[0], fielddefs, validAttributes))
                return false;

        }
        if ((features->featureTypes() & itLINE) != 0) {
            datasources[ilwisType2Index(itLINE)] =  createFileBasedDataSource("_lines", fileinfo);
        }
        if ((features->featureTypes() & itPOLYGON) != 0) {
            datasources[ilwisType2Index(itPOLYGON)] =  createFileBasedDataSource("_polygon", fileinfo);
        }
    }else {
        int typeIndex  = ilwisType2Index(types);
        datasources[typeIndex]._source =  createFileBasedDataSource("", fileinfo);
        if ( datasources[typeIndex]._source == 0)
            return false;
        datasources[typeIndex]._layers.push_back(createLayer(features->name(), ilwisType2GdalFeatureType(types), srs,datasources[typeIndex]._source));
        if (  datasources[typeIndex]._layers.back() == 0)
            return false;

        createAttributes(tbl, datasources[typeIndex]._layers[0], fielddefs,validAttributes);
    }

    for(OGRFieldDefnH fieldef : fielddefs) {
        gdal()->destroyAttributeDefintion(fieldef);
    }
    return true;
}

OGRGeometryH GdalFeatureConnector::createPoint2D(const SPFeatureI& feature) {
    OGRGeometryH hgeom = gdal()->createGeometry(wkbPoint);
    Coordinate2d crd = feature->geometry().toType<Coordinate>();
    gdal()->add2dPoint(hgeom,0, crd.x(), crd.y());
    return hgeom;
}

int GdalFeatureConnector::ilwisType2Index(IlwisTypes tp)
{
    switch (tp){
        case itPOINT:
            return 0;
        case itLINE:
            return 1;
        case itPOLYGON:
            return 2;
    }
    throw ErrorObject(TR("Illegal use of ilwisType2Index method"));
}

OGRGeometryH GdalFeatureConnector::createLine2D(const SPFeatureI& feature) {
    OGRGeometryH hgeom = gdal()->createGeometry(wkbLineString);
    Line2D<Coordinate2d> line = feature->geometry().toType<Line2D<Coordinate2d>>();
    for(int i=0; i < line.size(); ++i) {
        Coordinate crd = line[i];
        gdal()->add2dPoint(hgeom,i, crd.x(), crd.y());
    }
    return hgeom;

}

bool GdalFeatureConnector::oneLayerPerFeatureType(const IFeatureCoverage& features) {


    std::vector<SourceHandles> datasources(3);
    std::vector<bool> validAttributes;
    if(!setDataSourceAndLayers(features, datasources, validAttributes))
        return false;

    FeatureIterator fiter(features);
    FeatureIterator endIter = end(features);
    //std::for_each(fiter, fiter.end(), [&](const SPFeatureI& feature){
    for(; fiter != endIter; ++fiter) {
        SPFeatureI feature = *fiter;
        OGRLayerH lyr = datasources[ilwisType2Index(feature->ilwisType())]._layers[0];
        OGRFeatureH hfeature = gdal()->createFeature(gdal()->getLayerDef(lyr));
        setAttributes(hfeature, feature, validAttributes);
        OGRGeometryH hgeom = 0;
        if ( hasType(feature->geometry().ilwisType(), itPOINT)) {
            hgeom = createPoint2D(feature);
        }
        if ( hasType(feature->geometry().ilwisType(), itLINE)) {
            hgeom = createLine2D(feature);
        }
        gdal()->setGeometry(hfeature,hgeom);
        gdal()->destroyGeometry(hgeom);

        if ( hfeature) {
            if (gdal()->addFeature2Layer(lyr, hfeature) != OGRERR_NONE) {
                ERROR2(ERR_COULD_NOT_ALLOCATE_2, TR("feature"), features->name());
            }
            gdal()->destroyFeature(hfeature);
        };
    }
    for(auto& datasource : datasources){
        if ( datasource._source != 0)
            gdal()->destroyDataSource(datasource._source);
    }
    return true;
}

bool GdalFeatureConnector::loadDriver()
{

    _driver = gdal()->getDriverByName(_gdalShortName.toLocal8Bit());
    if ( !_driver ) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, "data-source", _filename.toString());
    }
    //int ret = gdal()->testDriverCapability(_driver,"ODrCCreateDataSource");
    //if ( ret == FALSE){
    //    return ERROR2(ERR_OPERATION_NOTSUPPORTED2, "write data-source", _filename.toString());
    //}


    return true;
}

bool GdalFeatureConnector::store(IlwisObject *obj, int )
{

    if (!loadDriver())
        return false;

    IFeatureCoverage features;
    features.set(static_cast<FeatureCoverage *>(obj));

    if(!oneLayerPerFeatureType(features))
        return false;

    return true;

}
