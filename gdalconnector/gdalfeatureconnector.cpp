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
    Resource resource(QUrl(QString("gdal://attributetable/%2/%1").arg(fcoverage->name()).arg(fcoverage->id())), itFLATTABLE);
    if(!attTable.prepare(resource)) {
        ERROR1(ERR_NO_INITIALIZED_1,resource.name());
        return false;
    }
    attTable->setParent(fcoverage);//to enable the GdalFeatureTableConnector to initiate loadBinaryData through this connector
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
                        domain.prepare("code=domain:text",itTEXTDOMAIN);
                        break;
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
                        ndomain->range(new TimeInterval());//TODO:: ilwisType of Domain should change to itTIMEDOMAIN
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

    QFileInfo fileinf = containerConnector()->toLocalFile(_filename);
    gdal()->closeFile(fileinf.absoluteFilePath(), data->id());

    return true;
}

QVariant GdalFeatureConnector::fillEmptyColumn(OGRFeatureH , int ){
    return QVariant();
}

QVariant GdalFeatureConnector::fillStringColumn(OGRFeatureH featureH, int colIntex){
    return QVariant(gdal()->getFieldAsString(featureH, colIntex));
}

QVariant GdalFeatureConnector::fillIntegerColumn(OGRFeatureH featureH, int colIntex){
    int v = gdal()->getFieldAsInt(featureH, colIntex);
    return QVariant(v);
}
//TODO:: not yet tested
QVariant GdalFeatureConnector::fillDoubleColumn(OGRFeatureH featureH, int colIntex){
    double v = gdal()->getFieldAsDouble(featureH, colIntex);
    return QVariant(v);
}
//TODO:: not yet tested
QVariant GdalFeatureConnector::fillDateTimeColumn(OGRFeatureH featureH, int colIntex){
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
    return QVariant(v);
}

bool GdalFeatureConnector::loadBinaryData(IlwisObject* data){

    if(!GdalConnector::loadMetaData(data))
        return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    if ( fcoverage->isValid() ) {
        ITable attTable = fcoverage->attributeTable();
        attTable->setParent(0);//to prevent the GdalFeatureTableConnector of attTable to start loading the binaryData again
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
                        ColumnDefinition& coldef = attTable->columndefinition(i);
                        if (coldef.name().compare(name,Qt::CaseSensitive) == 0){//check if column was deleted from metadata
                            DataDefinition& datadef = coldef.datadef();
                            if(datadef.domain().isValid()){
                                IlwisTypes tp = datadef.domain()->valueType();
                                if (tp & itSTRING){
                                    columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillStringColumn, i+offset);
                                }else if (tp & itINTEGER){
                                    NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
                                    //creating the actual range as invalid to be adjusted in the fillers
                                    double min = r->min();
                                    r->min(r->max());
                                    r->max(min);
                                    datadef.range(r);
                                    columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillIntegerColumn, i+offset);
                                }else if (tp & itDOUBLE){
                                    //creating the actual range as invalid to be adjusted in the fillers
                                    NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
                                    double min = r->min();
                                    r->min(r->max());
                                    r->max(min);
                                    datadef.range(r);
                                    columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillDoubleColumn, i+offset);
                                }else if (tp & itTIME){
                                    //creating the actual range as invalid to be adjusted in the fillers
                                    NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
                                    double min = r->min();
                                    r->min(r->max());
                                    r->max(min);
                                    datadef.range(r);
                                    columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillDateTimeColumn, i+offset);
                                }else{
                                    columnDefinitions[i] = nullptr;
                                }
                            }else{
                                columnDefinitions[i] = nullptr;
                            }
                        }else{//column was deleted from metadata
                            columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillEmptyColumn, -1);
                            offset++;
                        }
                    }else{//coulumn was added to metadata
                        columnDefinitions[i] = new FillerColumnDef(&GdalFeatureConnector::fillEmptyColumn, -1);
                    }
                }

                std::vector<QVariant> record;
                record.resize(attTable->columnCount());


                quint64 rec = 0;
                OGRFeatureH hFeature = 0;
                gdal()->resetReading(hLayer);
                //each FEATURE
                try {
                    while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL){
                        std::vector<Geometry> geometries;
                        fillFeature(fcoverage, gdal()->getGeometryRef(hFeature), geometries);
                        for(auto &geometry : geometries){

                            SPFeatureI& feature =  fcoverage->newFeature(geometry);
                            record[1] = QVariant(++rec);//COVERAGEKEYCOLUMN
                            //each OGR_F_FIELD > RECORD
                            for (int i = 2; i < attTable->columnCount();i++){
                                if (columnDefinitions[i]){
                                    record[i] = (this->*columnDefinitions[i]->fillFunc)(hFeature, columnDefinitions[i]->index);
                                }
                            }
                            record[0] = QVariant(feature->featureid());
                            attTable->record(attTable->recordCount() - 1,record);//should overwrite the last record in attTable, which hopefully is the one created by fcoverage->newFeature({geometry}) within FillFeature
                        }

                        gdal()->destroyFeature( hFeature );
                    }
                } catch (FeatureCreationError& ) {
                    gdal()->destroyFeature( hFeature );
                    return false;
                }

            }
//      }
            //TODO: this is only debug
            for (quint32 i = 0; i < attTable->columnCount();i++){
                ColumnDefinition& coldef = attTable->columndefinition(i);
                SPRange r = coldef.datadef().range();
            }

    }

    QFileInfo fileinf = containerConnector()->toLocalFile(_filename);
    gdal()->closeFile(fileinf.absoluteFilePath(), data->id());

    return true;
}

void GdalFeatureConnector::fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry,std::vector<Geometry>& outGeoms) const{
    if (geometry){
        OGRwkbGeometryType type = gdal()->getGeometryType(geometry);
        switch (translateOGRType(type)){
            case itUNKNOWN: ERROR2(ERR_INVALID_PROPERTY_FOR_2, "GeometryType of a Feature", _filename.toString()); return ;
            case itPOINT:   fillPoint(fcoverage, geometry, outGeoms);    break;
            case itLINE:    fillLine(fcoverage, geometry, outGeoms);     break;
            case itPOLYGON: fillPolygon(fcoverage, geometry, type,outGeoms);  break;
            default:{ //possibly wkbGeometryCollection or other kind of mixture
                int subGeomCount = gdal()->getSubGeometryCount(geometry);
                if(subGeomCount){
                    for(int i = 0; i < subGeomCount; ++i) {
                        OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
                        if(hSubGeometry){
                            fillFeature(fcoverage, hSubGeometry, outGeoms);
                        }
                    }
                    return ;
                }
                break;
            }
        }
    }

    return ;
}
void GdalFeatureConnector::fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry,std::vector<Geometry>& outGeoms ) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);
    if(subGeomCount > 0){
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                fillPoint(fcoverage, hSubGeometry, outGeoms);
            }
        }
        return ;
    }else{
        double x,y,z;
        gdal()->getPoints(geometry, 0,&x,&y,&z);
        Coordinate coord(x,y,z);
        Geometry point(coord, fcoverage->coordinateSystem());
        outGeoms.push_back(point);
    }
}
void GdalFeatureConnector::fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry,std::vector<Geometry>& outGeoms) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);
    if(subGeomCount > 0){
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                fillLine(fcoverage, hSubGeometry, outGeoms);
            }
        }
        return ;
    }else{
        int count = gdal()->getPointCount(geometry);
         if ( count == 0)
             return ;
         ITable attTable = fcoverage->attributeTable();
         if (!attTable.isValid())
             return ;

         Line2D line;
         line.resize(count);
         for(int i = 0; i < count; ++i) {
             double x,y,z;
             gdal()->getPoints(geometry, i,&x,&y,&z);
             line[i] = Coordinate2d(x,y);
//             attTable->setCell(FEATUREVALUECOLUMN, rec, QVariant(z));
         }
        outGeoms.push_back(Geometry(line, fcoverage->coordinateSystem()));
    }
}

void GdalFeatureConnector::fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry, OGRwkbGeometryType type,std::vector<Geometry>& outGeoms) const{
    int subGeomCount = gdal()->getSubGeometryCount(geometry);//error!check type!
    if ( type == wkbPolygon || type == wkbPolygon25D ){
        OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, 0);
        if(hSubGeometry){
            int count = gdal()->getPointCount(hSubGeometry);
            if ( count == 0)
                return ;
            ITable attTable = fcoverage->attributeTable();
            if (!attTable.isValid())
                return ;

            Polygon pol;
            std::vector<Coordinate2d>& ring = pol.outer();
            ring.resize(count);

            for(int i = 0; i < count; ++i) {
                double x,y,z;
                gdal()->getPoints(hSubGeometry, i,&x,&y,&z);
                ring[i] = Coordinate2d(x,y);
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
            outGeoms.push_back(Geometry(pol, fcoverage->coordinateSystem()));
        }else{
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"polygon for a record",_filename.toString());
            return ;
        }
    }else if (type == wkbMultiPolygon || type == wkbMultiPolygon25D){
        for(int i = 0; i < subGeomCount; ++i) {
            OGRGeometryH hSubGeometry = gdal()->getSubGeometryRef(geometry, i);
            if(hSubGeometry){
                fillPolygon(fcoverage, hSubGeometry, wkbPolygon, outGeoms);
            }
        }
    }else{
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"polygon for a record",_filename.toString());
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

void GdalFeatureConnector::setAttributes(OGRFeatureH hfeature, SPFeatureI& feature, const std::vector<bool>& validAttributes, const std::vector<ColumnDefinition>& columnDef) {
    int index = 0;
    for(int i=0; i < feature->attributeColumnCount(); ++i){
        if ( !validAttributes[i])
            continue;

        if(hasType(columnDef[i].datadef().domain()->valueType(),itINTEGER)) {
            qint32 val = feature->cell(columnDef[i].columnindex()).toInt();
            gdal()->setIntegerAttribute(hfeature,index,val);
        } else if (hasType(columnDef[i].datadef().domain()->valueType(),itDOUBLE | itFLOAT)) {
            double val = feature->cell(columnDef[i].columnindex()).toDouble();
            gdal()->setDoubleAttribute(hfeature,index,val);
        } else if (hasType(columnDef[i].datadef().domain()->valueType(),itDOMAINITEM)) {
            QVariant var = feature->cell(i);
            QString val = columnDef[i].datadef().domain()->value(var);
            gdal()->setStringAttribute(hfeature,index,val.toLocal8Bit());
        }
        //TODO: coords, time ..
        ++index;
    }

}

bool GdalFeatureConnector::createDataSourceAndLayers(IlwisTypes types,
                                                     const QString& postfix,
                                                     const IFeatureCoverage& features,
                                                     OGRSpatialReferenceH srs,
                                                     const std::vector<OGRFieldDefnH>& fielddefs,
                                                     std::vector<SourceHandles>& datasources,
                                                     std::vector<bool>& validAttributes ){

    QFileInfo fileinfo = containerConnector(IlwisObject::cmOUTPUT)->toLocalFile(_filename);
    int typeIndex  = ilwisType2Index(types);
    AttributeTable tbl = features->attributeTable();
    datasources[typeIndex]._source =  createFileBasedDataSource(postfix, fileinfo);
    if ( datasources[typeIndex]._source == 0)
        return false;
    datasources[typeIndex]._layers.push_back(createLayer(features->name(), ilwisType2GdalFeatureType(types), srs,datasources[typeIndex]._source));
    if (  datasources[typeIndex]._layers.back() == 0)
        return false;

    return createAttributes(tbl, datasources[typeIndex]._layers[0], fielddefs,validAttributes);
}

bool GdalFeatureConnector::setDataSourceAndLayers(const IFeatureCoverage& features, std::vector<SourceHandles>& datasources,std::vector<bool>& validAttributes) {


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
    bool ok = false;
    OGRSpatialReferenceH srs = createSRS(features->coordinateSystem());
    IlwisTypes types = features->featureTypes();
    bool multipleoutputs = (types == (itPOINT | itLINE)) || (types == (itPOINT | itPOLYGON)) || (types == (itLINE | itPOLYGON));
    if ( multipleoutputs){
        if ((features->featureTypes() & itPOINT) != 0) {
            ok = createDataSourceAndLayers(itPOINT, "point", features, srs,fielddefs,datasources,validAttributes);

        }
        if ((features->featureTypes() & itLINE) != 0) {
            ok = createDataSourceAndLayers(itLINE, "line", features, srs,fielddefs,datasources,validAttributes);
        }
        if ((features->featureTypes() & itPOLYGON) != 0) {
            ok = createDataSourceAndLayers(itPOLYGON, "polygon", features, srs,fielddefs,datasources,validAttributes);
        }
    }else {
        ok = createDataSourceAndLayers(types, "", features, srs,fielddefs,datasources,validAttributes);
    }

    for(OGRFieldDefnH fieldef : fielddefs) {
        gdal()->destroyAttributeDefintion(fieldef);
    }
    return ok;
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
    Line2D line = feature->geometry().toType<Line2D>();
    for(int i=0; i < line.size(); ++i) {
        Coordinate crd = line[i];
        gdal()->add2dPoint(hgeom,i, crd.x(), crd.y());
    }
    return hgeom;

}

OGRGeometryH GdalFeatureConnector::createPolygon2D(const SPFeatureI& feature){
    OGRGeometryH hgeom = gdal()->createGeometry(wkbPolygon);
    Polygon polygon = feature->geometry().toType<Polygon>();
    OGRGeometryH hgeomring = gdal()->createGeometry(wkbLinearRing);
    for(int i=0; i < polygon.outer().size(); ++i) {
        Coordinate crd = polygon.outer()[i];
        gdal()->add2dPoint(hgeomring,i, crd.x(), crd.y());
    }
    gdal()->add2Geometry(hgeom, hgeomring);
    gdal()->destroyGeometry(hgeomring);

    for(int hole=0; hole < polygon.inners().size(); ++hole){
        const std::vector<Coordinate2d>& holecrd = polygon.inners()[hole];
        OGRGeometryH hinnerring = gdal()->createGeometry(wkbLinearRing);
        for(int i=0; i < holecrd.size(); ++i) {
            Coordinate crd = holecrd[i];
            gdal()->add2dPoint(hinnerring,i, crd.x(), crd.y());
        }
        gdal()->add2Geometry(hgeom, hinnerring);
        gdal()->destroyGeometry(hinnerring);

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
    std::vector<ColumnDefinition> defs;
    for(int i=0; i < features->attributeTable()->columnCount(); ++i){
        defs.push_back(features->attributeTable()->columndefinition(i));
    }
    for(; fiter != endIter; ++fiter) {
        SPFeatureI& feature = *fiter;
        OGRLayerH lyr = datasources[ilwisType2Index(feature->geometryType())]._layers[0];
        OGRFeatureH hfeature = gdal()->createFeature(gdal()->getLayerDef(lyr));
        setAttributes(hfeature, feature, validAttributes, defs);
        OGRGeometryH hgeom = 0;
        if ( hasType(feature->geometry().geometryType(), itPOINT)) {
            hgeom = createPoint2D(feature);
        }
        if ( hasType(feature->geometry().geometryType(), itLINE)) {
            hgeom = createLine2D(feature);
        }
        if ( hasType(feature->geometry().geometryType(), itPOLYGON)) {
            hgeom = createPolygon2D(feature);
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
