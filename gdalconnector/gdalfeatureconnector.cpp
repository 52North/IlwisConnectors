#include <QSqlQuery>
#include <QSqlError>
#include <fstream>
#include <iterator>

#include "kernel.h"
#include "coverage.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/GeometryFactory.h"
#include "module.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "table.h"
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
#include "gdaltableloader.h"
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

ITable GdalFeatureConnector::prepareAttributeTable(OGRLayerH hLayer) const{

    ITable attTable;
    Resource resource(_filename, itFLATTABLE);
    if(!attTable.prepare(resource)) {
        ERROR1(ERR_NO_INITIALIZED_1,resource.name());
        return ITable();
    }
    IDomain dmKey;
    dmKey.prepare("count");
    ColumnDefinition colKey(FEATUREIDCOLUMN, dmKey, 0);
    attTable->addColumn(colKey);
    ColumnDefinition colCovKey(COVERAGEKEYCOLUMN, dmKey, 1);
    attTable->addColumn(colCovKey);

//    GdalTableLoader loader;
//    loader.loadMetaData(attTable.ptr(), hLayer);

    return attTable;

}
bool GdalFeatureConnector::loadMetaData(Ilwis::IlwisObject *data){

    if(!CoverageConnector::loadMetaData(data))
        return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IlwisTypes coverageType = itUNKNOWN;
    int featureCount = 0;
    Envelope bbox;
    bool initMinMax = 0;



    int layer = 0;//only first layer will be read/fit into single FeatureCoverage(*data)
    OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), layer);
    if ( hLayer) {
        ITable attTable = prepareAttributeTable(hLayer);
        if (!attTable.isValid())
            return false;

        fcoverage->attributeTable(attTable);

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
            bbox=Envelope(Coordinate(envelope.MinX,envelope.MinY),Coordinate(envelope.MaxX,envelope.MaxY));
        }else{
            if(bbox.max_corner().x < envelope.MaxX)
                bbox.max_corner().x = envelope.MaxX;
            if(bbox.max_corner().y < envelope.MaxY)
                bbox.max_corner().y = envelope.MaxY;
            if(bbox.min_corner().x > envelope.MinX)
                bbox.min_corner().x = envelope.MinX;
            if(bbox.min_corner().y > envelope.MinY)
                bbox.min_corner().y = envelope.MinY;
        }
    }
    //    }
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

bool GdalFeatureConnector::loadBinaryData(IlwisObject* data){

    if(!GdalConnector::loadMetaData(data))
        return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    if ( fcoverage->isValid() ) {
        ITable attTable = fcoverage->attributeTable();
        if (!attTable.isValid()){
            ERROR2(ERR_NO_INITIALIZED_2,"attribute table",_filename.toString());
            return false;
        }
        fcoverage->setFeatureCount(itPOLYGON, 0); // metadata already set it to correct number, creating new features will up the count agains; so reset to 0.
        fcoverage->setFeatureCount(itLINE, 0);
        fcoverage->setFeatureCount(itPOINT, 0);

        //each LAYER
        int layer = 0;//only the first layer fits into one FeatureCoverage
        OGRLayerH hLayer = gdal()->getLayer(_handle->handle(), layer);
        if ( hLayer) {
            GdalTableLoader loader;
            attTable->dataLoaded(true); // new table, dont want any loading behaviour

            loader.setColumnCallbacks(attTable.ptr(), hLayer);
            std::vector<QVariant> record(attTable->columnCount());
            OGRFeatureH hFeature = 0;
            gdal()->resetReading(hLayer);
            //each FEATURE
            quint32 keyindex = attTable->columnIndex(COVERAGEKEYCOLUMN);
            try {
                quint32 count = 0;
                while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL){
                    loader.loadRecord(attTable.ptr(), hFeature, record);
                    std::vector<geos::geom::Geometry *> geometries;
                    fillFeature(fcoverage, gdal()->getGeometryRef(hFeature), geometries);
                    record[keyindex] = count++;
                    for(auto &geometry : geometries){
                        attTable->record(attTable->recordCount(), record);
                        fcoverage->newFeature(geometry, false);
                    }

                    gdal()->destroyFeature( hFeature );
                }
            } catch (FeatureCreationError& ) {
                gdal()->destroyFeature( hFeature );
                return false;
            }

        }

    }

    QFileInfo fileinf = containerConnector()->toLocalFile(_filename);
    gdal()->closeFile(fileinf.absoluteFilePath(), data->id());
    _binaryIsLoaded = true;
    return true;
}

void GdalFeatureConnector::fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry,std::vector<geos::geom::Geometry*>& outGeoms) const{
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
void GdalFeatureConnector::fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry,std::vector<geos::geom::Geometry*>& outGeoms ) const{
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
        geos::geom::Point *point = fcoverage->geomfactory()->createPoint(coord);
        //fcoverage->newFeature(point);
        outGeoms.push_back(point);
    }
}
void GdalFeatureConnector::fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry,std::vector<geos::geom::Geometry*>& outGeoms) const{
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

         geos::geom::CoordinateArraySequence *vertices = new geos::geom::CoordinateArraySequence(count);
         for(int i = 0; i < count; ++i) {
             double x,y,z;
             gdal()->getPoints(geometry, i,&x,&y,&z);
             vertices->setAt(geos::geom::Coordinate(x,y), i);
//             attTable->setCell(FEATUREVALUECOLUMN, rec, QVariant(z));
         }
        geos::geom::LineString *line = fcoverage->geomfactory()->createLineString(vertices);
        //fcoverage->newFeature(line);
        outGeoms.push_back(line);
    }
}

void GdalFeatureConnector::fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry, OGRwkbGeometryType type,std::vector<geos::geom::Geometry*>& outGeoms) const{
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

            geos::geom::CoordinateArraySequence *outer = new geos::geom::CoordinateArraySequence(count);

            for(int i = 0; i < count; ++i) {
                double x,y,z;
                gdal()->getPoints(hSubGeometry, i,&x,&y,&z);
                outer->setAt(geos::geom::Coordinate(x,y),i);
            }

            geos::geom::LinearRing *outerring = fcoverage->geomfactory()->createLinearRing(outer);
            std::vector<geos::geom::Geometry*> *inners = new std::vector<geos::geom::Geometry*>(subGeomCount - 1);
            for(int j = 1; j < subGeomCount; ++j) {
                hSubGeometry = gdal()->getSubGeometryRef(geometry, j);
                if(hSubGeometry){
                    count = gdal()->getPointCount(hSubGeometry);
                    if(count == 0)
                       continue;
                    geos::geom::CoordinateArraySequence* innerring = new geos::geom::CoordinateArraySequence(count);
                    for(int i = 0; i < count; ++i) {
                        double x,y,z;
                        gdal()->getPoints(hSubGeometry, i,&x,&y,&z);
                        innerring->setAt(geos::geom::Coordinate(x,y), i);
                    }
                    (*inners)[j-1] = fcoverage->geomfactory()->createLinearRing(innerring);
                }
            }
            geos::geom::Polygon *pol = fcoverage->geomfactory()->createPolygon(outerring, inners);
            outGeoms.push_back(pol);
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

void GdalFeatureConnector::setAttributes(OGRFeatureH hfeature, UPFeatureI& feature, const std::vector<bool>& validAttributes, const std::vector<ColumnDefinition>& columnDef) {
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

OGRGeometryH GdalFeatureConnector::createPoint2D(const UPFeatureI& feature) {
    OGRGeometryH hgeom = gdal()->createGeometry(wkbPoint);
    const geos::geom::Coordinate *crd = feature->geometry()->getCoordinate();
    gdal()->add2dPoint(hgeom,0, crd->x, crd->y);
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

OGRGeometryH GdalFeatureConnector::createLine2D(const UPFeatureI& feature) {
    OGRGeometryH hgeom = gdal()->createGeometry(wkbLineString);
    std::unique_ptr<geos::geom::CoordinateSequence> coords(feature->geometry()->getCoordinates());
    for(int i=0; i < coords->size(); ++i) {
        geos::geom::Coordinate crd = coords->getAt(i);
        gdal()->add2dPoint(hgeom,i, crd.x, crd.y);
    }
    return hgeom;

}

OGRGeometryH GdalFeatureConnector::createPolygon2D(const UPFeatureI& feature){
    OGRGeometryH hgeom = gdal()->createGeometry(wkbPolygon);
    geos::geom::Polygon *polygon = dynamic_cast<geos::geom::Polygon *>(feature->geometry().get());
    OGRGeometryH hgeomring = gdal()->createGeometry(wkbLinearRing);
    const geos::geom::CoordinateSequence *outer = polygon->getExteriorRing()->getCoordinatesRO();
    for(int i=0; i < outer->size(); ++i) {
        Coordinate crd = outer->getAt(i);
        gdal()->add2dPoint(hgeomring,i, crd.x, crd.y);
    }
    gdal()->add2Geometry(hgeom, hgeomring);
    gdal()->destroyGeometry(hgeomring);

    for(int hole=0; hole < polygon->getNumInteriorRing(); ++hole){
        const geos::geom::CoordinateSequence *inner = polygon->getInteriorRingN(hole)->getCoordinatesRO();
        OGRGeometryH hinnerring = gdal()->createGeometry(wkbLinearRing);
        for(int i=0; i < inner->size(); ++i) {
            Coordinate crd = inner->getAt(i);
            gdal()->add2dPoint(hinnerring,i, crd.x, crd.y);
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
        UPFeatureI& feature = *fiter;
        OGRLayerH lyr = datasources[ilwisType2Index(feature->geometryType())]._layers[0];
        OGRFeatureH hfeature = gdal()->createFeature(gdal()->getLayerDef(lyr));
        setAttributes(hfeature, feature, validAttributes, defs);
        OGRGeometryH hgeom = 0;
        if ( feature->geometry()->getGeometryTypeId() == geos::geom::GEOS_POINT) {
            hgeom = createPoint2D(feature);
        }
        if ( feature->geometry()->getGeometryTypeId() == geos::geom::GEOS_LINESTRING) {
            hgeom = createLine2D(feature);
        }
        if ( feature->geometry()->getGeometryTypeId() == geos::geom::GEOS_POLYGON) {
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
