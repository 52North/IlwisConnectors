#include <QSqlQuery>
#include <QSqlError>
#include <fstream>
#include <iterator>

#include "coverage.h"
#include "boost/math/special_functions.hpp"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/GeometryFactory.h"
#ifdef Q_OS_WIN
#include "geos/geom/Coordinate.inl"
#include "geos/geom/Envelope.inl"
#endif
#include "module.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "table.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "containerstatistics.h"
#include "rawconverter.h"
#include "ilwis3connector.h"
#include "binaryilwis3table.h"
#include "coordinatedomain.h"
#include "coverageconnector.h"
#include "tableconnector.h"
#include "ilwis3featureconnector.h"


using namespace Ilwis;
using namespace Ilwis3;

struct XYZ {
    double x,y,z;
};

ConnectorInterface *FeatureConnector::create(const Resource &resource, bool load,const IOOptions& options) {
    return new FeatureConnector(resource, load, options);

}

Ilwis::IlwisObject *FeatureConnector::create() const
{
    return new Ilwis::FeatureCoverage(_resource);
}

void FeatureConnector::calcStatics(const IlwisObject *obj, NumericStatistics::PropertySets set) const
{
}

FeatureConnector::FeatureConnector(const Resource &resource, bool load, const IOOptions &options) : CoverageConnector(resource, load, options)
{
}

bool FeatureConnector::loadBinaryPolygons30(FeatureCoverage *fcoverage, ITable& tbl) {
    BinaryIlwis3Table polTable;
    if ( !polTable.load(_odf)) {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->url())    ;
    }

    BinaryIlwis3Table topTable;
    if ( !topTable.load(_odf,"top")) {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->url())    ;
    }

    qint32 colValue = polTable.index("PolygonValue");
    qint32 colTopStart = polTable.index("TopStart");
    qint32 colArea = polTable.index("Area");
    int nrPolygons = polTable.rows();
    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;
    fcoverage->setFeatureCount(itFEATURE, iUNDEF, FeatureInfo::ALLFEATURES); // reset all counts

    double v;
    for(int i = 0; i < nrPolygons; ++i) {
        polTable.get(i,colArea, v);
        if ( v < 0)
            continue;
        polTable.get(i,colTopStart,v);
        qint32 index = v;
        std::vector<geos::geom::LinearRing *> *rings = new std::vector<geos::geom::LinearRing *>();
        if (getRings(fcoverage,index, topTable, polTable, rings)) {
            if ( rings->size() == 0)
                continue;
            geos::geom::LinearRing *outer = rings->front();
            vector<geos::geom::Geometry *> *geoms = new vector<geos::geom::Geometry *>(rings->size() - 1);
            for(int j=1; j < rings->size(); ++j) {
                (*geoms)[j-1] = rings->at(j);
            }

            geos::geom::Polygon *polygon = fcoverage->geomfactory()->createPolygon(outer, geoms);
            //std::copy(rings[0].begin(), rings[0].end(), polygon.outer().begin());
            polTable.get(i, colValue, v);
            if ( isNumeric) {
                fcoverage->newFeature({polygon}, false);
                tbl->setCell(COVERAGEKEYCOLUMN, i, QVariant(i));
                tbl->setCell(FEATUREVALUECOLUMN, i, QVariant(v));
            } else {
                fcoverage->newFeature({polygon}, false);
                quint32 itemId = v;
                tbl->setCell(COVERAGEKEYCOLUMN, i, QVariant(itemId - 1));
            }
        }
    }
    return true;
}

bool FeatureConnector::getRings(FeatureCoverage *fcoverage, qint32 startIndex, const BinaryIlwis3Table& topTable, const BinaryIlwis3Table& polTable, std::vector<geos::geom::LinearRing *> *rings ){
    qint32 row = startIndex;
    qint32 colCoords = topTable.index("Coords");
    qint32 colForward = topTable.index("ForwardLink");
    qint32 colBackward = topTable.index("BackwardLink");
    std::vector<geos::geom::Coordinate> *ring;
    bool forward = isForwardStartDirection(topTable,colForward, colBackward, colCoords, row);
    do{
        std::vector<Coordinate> coords;
        topTable.get(abs(row) - 1,colCoords,coords);
        if( coords.size() == 0 ||coords.back() == coords.front()){
            ring = new std::vector<geos::geom::Coordinate>(coords.size());
            std::copy(coords.begin(), coords.end(), ring->begin());
        } else if ( ring->size() > 0 && coords.back() == ring->back()) {
            ring = new std::vector<geos::geom::Coordinate>(coords.size());
            std::reverse_copy(coords.begin(), coords.end(), ring->begin());
        } else if ( ring->size() > 0 && ring->front() == coords.front()) {
            ring = new std::vector<geos::geom::Coordinate>(coords.size());
            std::reverse_copy(coords.begin(), coords.end(), ring->begin());
        } else if ( ring->size() > 0 && ring->front() == coords.back()) {
            ring = new std::vector<geos::geom::Coordinate>(coords.size());
            std::copy(coords.begin(), coords.end(), ring->begin());
        }

        if ( ring->size() > 3 && ring->front() == ring->back()) {
            geos::geom::CoordinateArraySequence * ringIn = new geos::geom::CoordinateArraySequence(ring);
            ringIn->removeRepeatedPoints();
            rings->push_back( fcoverage->geomfactory()->createLinearRing(ringIn));
            coords.clear();
        }
        qint32 oldIndex = row;
        double v;
        if ( forward)
            topTable.get(abs(row) - 1,colForward,v);
        else
            topTable.get(abs(row) - 1, colBackward, v);\
        row = v;
        if ( oldIndex == row && row != startIndex) // this would indicate infintite loop. corrupt data
            return false;
        forward = row > 0;
    } while(abs(row) != abs(startIndex) && row != iUNDEF);

    return true;
}

bool FeatureConnector::isForwardStartDirection(const BinaryIlwis3Table& topTable, qint32 colForward, qint32 colBackward, qint32 colCoords, long index) {
    qint32 fwl, bwl;
    double v;
    topTable.get(abs(index),colForward, v );
    fwl = v;
   // if ( fwl != iUNDEF)
   //     --fwl; // due to being raw values
    topTable.get(abs(index),colBackward, v );
    bwl = v;
   // if ( bwl != iUNDEF)
   //    --bwl;

    if ( abs(fwl) == abs(bwl)	)
        return true;
    if ( index < 0)
        return false;
    //qDebug() << fwl << bwl;
    std::vector<Coordinate> startLine, forwardLine;
    topTable.get(abs(index), colCoords,startLine);
    topTable.get(abs(fwl), colCoords, forwardLine);

    bool forward = false;
    if ( forwardLine.size() == 0 || startLine.size() == 0)
        throw ErrorObject(TR("aborting topoplogy reading, polygon might be corrupted"));

    if ( fwl > 0)
        forward =  startLine.back() == forwardLine.front();
    else
        forward = startLine.back() == forwardLine.back();

    return forward;

}


bool FeatureConnector::loadBinaryPolygons37(FeatureCoverage *fcoverage, ITable& tbl) {
    QString datafile = _odf->value("PolygonMapStore","DataPol");
    datafile = QFileInfo(QUrl(_odf->url()).toLocalFile()).absolutePath() + "/" + datafile;
    QFile file(datafile);

    if (!file.exists()){
        kernel()->issues()->log(TR(ERR_MISSING_DATA_FILE_1).arg(file.fileName()));
        return false;
    }
    if(!file.open(QIODevice::ReadOnly )){
        kernel()->issues()->log(TR(ERR_COULD_NOT_OPEN_READING_1).arg(file.fileName()));
        return false;
    }
    QDataStream stream(&file);
    int nrPolygons = fcoverage->featureCount(itPOLYGON);
    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;
    map<quint32,vector<geos::geom::Geometry *>> polygons;
    std::vector<double> featureValues(isNumeric ? nrPolygons : 0);
    fcoverage->setFeatureCount(itFEATURE, iUNDEF, FeatureInfo::ALLFEATURES); // reset all counts
    for(int j=0; j < nrPolygons; ++j) {
        double outerArea = rUNDEF;
        geos::geom::CoordinateArraySequence *outer = readRing(stream, outerArea);
        if ( !outer)
            return false;

        geos::geom::LinearRing *outerring = fcoverage->geomfactory()->createLinearRing(outer);
        double value;

        quint32 numberOfHoles;
        stream.readRawData((char *)&value, 8);
        stream.readRawData((char *)&numberOfHoles, 4);
        std::vector<geos::geom::Geometry*> *inners = new std::vector<geos::geom::Geometry*>();
        for(quint32 i=0; i< numberOfHoles;++i){
            auto ring = readRing(stream, outerArea);
            if ( ring){
                inners->push_back(fcoverage->geomfactory()->createLinearRing(ring));
            }
        }
        geos::geom::Polygon *pol = fcoverage->geomfactory()->createPolygon(outerring, inners);

        //collect all polygons in a map; the key is either a unique number ( index j) or the raw value from the polygon
        //in this way all polygons with the same raw value will become a multipolygon
        polygons[isNumeric ? j + 1 : (quint32)value].push_back(pol);
        if ( isNumeric) {
            featureValues[j] = value;
        }
    }
    addFeatures(polygons, fcoverage,featureValues, itPOLYGON);
    file.close();

    return true;
}

void  FeatureConnector::addFeatures(map<quint32,vector<geos::geom::Geometry *>>& geometries,FeatureCoverage *fcoverage,const std::vector<double>& featureValues, IlwisTypes tp) {
    quint32 rec = 0;
    ColumnDefinition& coldef = fcoverage->attributeDefinitionsRef().columndefinitionRef(featureValues.size() > 0 ? FEATUREVALUECOLUMN : COVERAGEKEYCOLUMN);
    coldef.datadef().range(Range::create(coldef.datadef().range<>()->valueType())); // resetting the range to a default state
    for(auto iter = geometries.begin() ; iter != geometries.end(); ++iter) {
        vector<geos::geom::Geometry *>& geoms1 = (*iter).second;
        geos::geom::Geometry *geometry = 0;
        if ( geoms1.size() == 1) {
            geometry = geoms1[0];
        } else {
            std::vector<geos::geom::Geometry *> *geoms = new std::vector<geos::geom::Geometry *>((*iter).second);
//            geos::geom::GeometryCollection *multigeom = 0;
            switch(tp) {
                case itPOLYGON:
                    geometry = fcoverage->geomfactory()->createMultiPolygon(geoms); break;
                case itLINE:
                    geometry = fcoverage->geomfactory()->createMultiLineString(geoms); break;
                case itPOINT:
                    geometry = fcoverage->geomfactory()->createMultiPoint(geoms); break;
            }


//            fcoverage->newFeature({multigeom},false);
        }
        auto feature = fcoverage->newFeature({geometry},false);
        if ( featureValues.size() > 0){
            QVariant value(featureValues[rec]);
            feature(FEATUREVALUECOLUMN, value);
        }
        else{
            QVariant value( (*iter).first == 0 ? (quint32)iUNDEF : (*iter).first - 1);
            feature(COVERAGEKEYCOLUMN, value);
        }

        ++rec;
    }
    if ( featureValues.size() == 0 && coldef.datadef().domain()->valueType() == itINDEXEDITEM){
        coldef.datadef().range<IndexedIdentifierRange>()->add(new IndexedIdentifier("",0,rec));
    }
}

geos::geom::CoordinateArraySequence* FeatureConnector::readRing(QDataStream& stream, double& outerArea ) {
    quint32 numberOfCoords;

    if (stream.readRawData((char *)&numberOfCoords, 4) <= 0){
        ERROR1(ERR_COULD_NOT_OPEN_READING_1,"data file");
        return 0;
    }
    vector<XYZ> pnts(numberOfCoords);
    stream.readRawData((char *)&pnts[0],numberOfCoords*3*8);
    double area = 0;
    double j = numberOfCoords - 1;
    geos::geom::CoordinateArraySequence *ring = new geos::geom::CoordinateArraySequence(numberOfCoords);
    for(quint32 i=0; i < numberOfCoords; ++i) {
        ring->setAt(geos::geom::Coordinate(pnts[i].x, pnts[i].y, pnts[i].z), i);
        area += (pnts[j].x + pnts[i].x) * (pnts[j].y - pnts[i].y);
        j = i;
    }
    if ( outerArea !=rUNDEF){
        if ( std::abs(area) < 1e-6){
            delete ring;
            return 0;
        }
        if ( boost::math::sign(outerArea) == boost::math::sign(area)){ // inner and outer rings must be in reverse order
            geos::geom::CoordinateSequence::reverse(ring);
        }
    }else
        outerArea = area;



   return ring;
}

bool FeatureConnector::loadBinaryPolygons(FeatureCoverage *fcoverage) {
    QString dataFile = _odf->value("PolygonMapStore","DataPol");
    ITable tbl = fcoverage->attributeTable();
    if ( dataFile == sUNDEF) {
        return loadBinaryPolygons30(fcoverage, tbl);
    } else {
        return loadBinaryPolygons37(fcoverage, tbl);
    }
    return false;
}

bool FeatureConnector::loadBinarySegments(FeatureCoverage *fcoverage) {
    BinaryIlwis3Table mpsTable;
    if ( !mpsTable.load(_odf)) {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->url())    ;
    }
    int colCoords = mpsTable.index("Coords");
    int colItemId = mpsTable.index("SegmentValue");
    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;
    fcoverage->setFeatureCount(itFEATURE, iUNDEF, FeatureInfo::ALLFEATURES); // reset all counts

    map<quint32,vector<geos::geom::Geometry *>> lines;
    std::vector<double> featureValues(isNumeric ? mpsTable.rows() : 0);
    double value;
    for(quint32 i= 0; i < mpsTable.rows(); ++i) {
        std::vector<Coordinate > coords;
        mpsTable.get(i,colCoords,coords);
        geos::geom::CoordinateArraySequence *vertices = new geos::geom::CoordinateArraySequence(coords.size());
        for(int i=0; i < coords.size(); ++i)
            vertices->setAt(geos::geom::Coordinate(coords[i].x, coords[i].y, coords[i].z), i);
        geos::geom::LineString *line = fcoverage->geomfactory()->createLineString(vertices);
        mpsTable.get(i, colItemId,value);

        //collect all lines in a map; the key is either a unique number ( index j) or the raw value from the line
        //in this way all lines with the same raw value will become a multilinestring
        lines[isNumeric ? i + 1 : (quint32)value].push_back(line);
        if ( isNumeric) {
            featureValues[i] = value;
        }


    }
    addFeatures(lines, fcoverage,featureValues, itLINE);
    return true;


}

bool FeatureConnector::loadBinaryPoints(FeatureCoverage *fcoverage) {
    BinaryIlwis3Table mppTable;
    if ( !mppTable.load(_odf)) {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->url())    ;
    }
    // two cases; the old case; 2 columns for x and y. and the new case one column for coord
    int coordColumnX = mppTable.index("x");
    int coordColumnY = mppTable.index("y");
    int coordColumn = mppTable.index("Coordinate");
    int colItemId = mppTable.index("Name");
    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;

    std::vector<double> featureValues(isNumeric ? mppTable.rows() : 0);
    bool newCase =  coordColumnX == iUNDEF;
    fcoverage->setFeatureCount(itFEATURE, iUNDEF, FeatureInfo::ALLFEATURES); // reset all counts

    map<quint32,vector<geos::geom::Geometry *>> points;
    for(quint32 i= 0; i < mppTable.rows(); ++i) {
        Coordinate c;
        double value;
        if ( newCase) {
            mppTable.get(i, coordColumn, c);
        } else {
            double x,y;
            mppTable.get(i, coordColumnX, x);
            mppTable.get(i, coordColumnY, y);
            c = Coordinate(x,y);
        }
        mppTable.get(i, colItemId,value);
        geos::geom::Point *point = fcoverage->geomfactory()->createPoint(c);
        points[isNumeric ? i + 1 : (quint32)value].push_back(point);
        if ( isNumeric) {
            featureValues[i] = value;
        }

    }
    addFeatures(points, fcoverage,featureValues, itPOINT);
    return true;
}

bool FeatureConnector::loadData(Ilwis::IlwisObject *obj, const IOOptions &) {
    if ( obj == nullptr)
        return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);

    QString file = _odf->value("BaseMap", "AttributeTable");
    ITable extTable;
    if ( file != sUNDEF) {
        if(!extTable.prepare(file)){
            kernel()->issues()->log(file,TR(ERR_NO_INITIALIZED_1).arg(file),IssueObject::itWarning);
            return false;
        }
    }
    bool ok = false;

    try {
        _binaryIsLoaded = true; // to prevent any subsequent calls to this routine while loading (which mat trigger it).
         if (fcoverage->featureTypes() == itPOINT)
             ok = loadBinaryPoints(fcoverage);
         else if (fcoverage->featureTypes() == itLINE)
             ok = loadBinarySegments(fcoverage);
         else if (fcoverage->featureTypes() == itPOLYGON)
             ok = loadBinaryPolygons(fcoverage);

         _binaryIsLoaded = ok;

         if ( ok && extTable.isValid()) {
             ITable attTbl = fcoverage->attributeTable();
            // quint32 keyIndex = extTable->columnIndex(COVERAGEKEYCOLUMN);
             for(quint32 rowExt=0; rowExt < extTable->recordCount(); ++rowExt) {
                 if ( rowExt < fcoverage->featureCount()){
                     vector<QVariant> rec = extTable->record(rowExt);
                     attTbl->record(rowExt,rec);
                 }
             }
         }
    } catch (FeatureCreationError& ) {
    }
    if ( ok)
        _binaryIsLoaded = true;
    return ok;
}

bool FeatureConnector::loadMetaData(Ilwis::IlwisObject *obj,const IOOptions& options)
{
    bool ok = CoverageConnector::loadMetaData(obj, options);
    if ( !ok)
        return false;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);
    fcoverage->setFeatureCount(itFEATURE, iUNDEF, FeatureInfo::ALLFEATURES);
    IlwisTypes coverageType = itPOINT;

    int features = _odf->value("PointMap","Points").toInt(&ok);
    if (!ok) {
        coverageType = itPOLYGON;
        features = _odf->value("PolygonMapStore","Polygons").toInt(&ok);
        if (!ok) {
            coverageType = itLINE;
            features = _odf->value("SegmentMapStore","Segments").toInt(&ok);
        }
    }

    if (ok){
        fcoverage->featureTypes(coverageType);
        fcoverage->setFeatureCount(coverageType, features,0);
    }
    else
       return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Records",obj->name());

    //ITable tbl = fcoverage->attributeTable();
    //tbl->recordCount(fcoverage->featureCount(itFEATURE,true));
    return true;

}

void FeatureConnector::writePolygon(const geos::geom::Polygon* polygon, std::ofstream& output_file, double raw){
    if (!polygon)
        return;

    const std::vector<geos::geom::Coordinate>* coords = polygon->getExteriorRing()->getCoordinates()->toVector();
    writeCoords(output_file, coords);
    output_file.write((char *)&raw,8);
    quint32 holeCount = polygon->getNumInteriorRing();
    output_file.write((char *)&holeCount,4);
    for(int i=0; i < holeCount; ++i ) {
        const std::vector<geos::geom::Coordinate>* coords = polygon->getInteriorRingN(i)->getCoordinates()->toVector();
        writeCoords(output_file, coords);
    }
}

bool FeatureConnector::storeBinaryDataPolygon(FeatureCoverage *fcov, const QString& baseName) {
    QString filename = baseName + ".mpz#";

    std::ofstream output_file(filename.toLatin1(),ios_base::out | ios_base::binary | ios_base::trunc);
    if ( !output_file.is_open())
        return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,filename);
    IFeatureCoverage cov;
    cov.set(fcov);
    FeatureIterator iter(cov);
    double raw = 1;
    for_each(iter, iter.end(), [&](SPFeatureI feature){
        if ( feature->geometry().get() != 0){
            storePolygon(feature->geometry(), fcov,output_file,raw);
            auto indexes = fcov->attributeDefinitions().indexes();
            for(auto index : indexes) {
                auto subfeature = feature[index];
                if (!subfeature.get()) {
                    continue;
                }
                storePolygon(subfeature->geometry(), fcov,output_file,raw);

            }
        }
        return true;
    });

    output_file.close();

    return true;
}

void FeatureConnector::storePolygon(const UPGeometry& geom, const FeatureCoverage *fcov, std::ofstream& output_file, double& raw){
    geos::geom::GeometryTypeId geostype = geom->getGeometryTypeId();
    if ( geostype == geos::geom::GEOS_POLYGON || geostype == geos::geom::GEOS_MULTIPOLYGON) {
        const geos::geom::Geometry *polygons = geom.get();
        if ( geostype == geos::geom::GEOS_POLYGON ){
            const geos::geom::Polygon *polygon = dynamic_cast<const geos::geom::Polygon*>(polygons);
            if ( !polygon){
                ERROR2(ERR_NO_INITIALIZED_2, "polygon", fcov->name());
                return;
            }
            writePolygon(polygon,output_file,raw);
        } else {
            int n = polygons->getNumGeometries();
            for(int i = 0; i < n ; ++i){
                const geos::geom::Polygon* polygon = dynamic_cast<const geos::geom::Polygon*>(polygons->getGeometryN(i));
                if ( !polygon){
                    ERROR2(ERR_NO_INITIALIZED_2, "polygon", fcov->name());
                    return;
                }
                writePolygon(polygon,output_file,raw);
            }
        }

        ++raw;
    }
}

void FeatureConnector::writeLine(const geos::geom::LineString* line,std::ofstream& output_file,long raw ) {
    if (!line)
        return;
    const geos::geom::Envelope *env = line->getEnvelopeInternal();
    const std::vector<geos::geom::Coordinate>* coords = line->getCoordinatesRO()->toVector();
    const geos::geom::Coordinate crdmin(env->getMinX(), env->getMinY());
    const geos::geom::Coordinate crdmax(env->getMaxX(), env->getMaxY());
    writeCoord(output_file, crdmin);
    writeCoord(output_file, crdmax);
    int noOfCoordsBytes = line->getNumPoints() * 16;
    output_file.write((char *)&noOfCoordsBytes, 4);
    for(const Coordinate& crd: *coords) {
        writeCoord(output_file, geos::geom::Coordinate(crd));
    }
    long deleted=1;
    output_file.write((char *)&deleted, 4);
    output_file.write((char *)&raw, 4);
}

bool FeatureConnector::storeBinaryDataLine(FeatureCoverage *fcov, const QString& baseName) {
    std::ofstream output_file;
    QString filename = baseName + ".mps#";
    output_file.open(filename.toLatin1(),ios_base::out | ios_base::binary | ios_base::trunc);
    if ( !output_file.is_open())
        return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,baseName);
    char header[128];
    memset(header, 0, 128);
    output_file.write(header,128);

    IFeatureCoverage cov;
    cov.set(fcov);
    FeatureIterator iter(cov);
    double raw = 1;

    for_each(iter, iter.end(), [&](SPFeatureI feature){
        storeSegment(feature->geometry(), fcov,output_file,raw);
        auto indexes = fcov->attributeDefinitions().indexes();
        for(auto index : indexes) {
            auto subfeature = feature[index];
            if (!subfeature.get()) {
                continue;
            }
            storeSegment(subfeature->geometry(), fcov,output_file,raw);

        }
        return true;
    });

    output_file.close();

    return true;
}

void FeatureConnector::storeSegment(const UPGeometry& geom, const FeatureCoverage *fcov, std::ofstream& output_file, double& raw){
    geos::geom::GeometryTypeId geostype = geom->getGeometryTypeId();
    if ( geostype == geos::geom::GEOS_MULTILINESTRING || geos::geom::GEOS_LINESTRING) {

        const geos::geom::Geometry *lines = geom.get();
        if ( geostype == geos::geom::GEOS_LINESTRING ){
            const geos::geom::LineString *line = dynamic_cast<const geos::geom::LineString*>(lines);
            if ( !line){
                ERROR2(ERR_NO_INITIALIZED_2, "lines", fcov->name());
                return;
            }
            writeLine(line,output_file,raw);
        } else {
            int n = lines->getNumGeometries();
            for(int i = 0; i < n ; ++i){
                const geos::geom::LineString *line = dynamic_cast<const geos::geom::LineString*>(lines->getGeometryN(i));
                if ( !line){
                    ERROR2(ERR_NO_INITIALIZED_2, "lines", fcov->name());
                    return;
                }
                writeLine(line,output_file,raw);
            }
        }
        ++raw;
    }
}
void FeatureConnector::writePoint(const geos::geom::Point* point,std::ofstream& output_file,long raw ) {
    writeCoord(output_file, *(point->getCoordinate()));
    output_file.write((char *)&raw, 4);
}

bool FeatureConnector::storeBinaryDataPoints(FeatureCoverage *fcov, const QString& baseName) {
    std::ofstream output_file;
    QString filename = baseName + ".pt#";
    output_file.open(filename.toLatin1(),ios_base::out | ios_base::binary | ios_base::trunc);
    if ( !output_file.is_open())
        return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,baseName);
    char header[128];
    memset(header, 0, 128);
    output_file.write(header,128);

    IFeatureCoverage cov;
    cov.set(fcov);
    FeatureIterator iter(cov);
    double raw = 1;

    for_each(iter, iter.end(), [&](SPFeatureI feature){
        storePoint(feature->geometry(), fcov,output_file,raw);
        auto indexes = fcov->attributeDefinitions().indexes();
        for(auto index : indexes) {
            auto subfeature = feature[index];
            if (!subfeature.get()) {
                continue;
            }
            storePoint(subfeature->geometry(), fcov,output_file,raw);

        }
        return true;
    });

    output_file.close();

    return true;
}

void FeatureConnector::storePoint(const UPGeometry& geom, const FeatureCoverage *fcov, std::ofstream& output_file, double& raw){
    geos::geom::GeometryTypeId geostype = geom->getGeometryTypeId();
    if ( geostype == geos::geom::GEOS_MULTIPOINT ||geostype == geos::geom::GEOS_POINT) {

        const geos::geom::Geometry *points = geom.get();
        if ( geostype == geos::geom::GEOS_POINT ){
            const geos::geom::Point *point = dynamic_cast<const geos::geom::Point*>(points);
            if ( !point){
                ERROR2(ERR_NO_INITIALIZED_2, "points", fcov->name());
                return;
            }
            writePoint(point,output_file, raw);
        } else {
            int n = points->getNumGeometries();
            for(int i = 0; i < n ; ++i){
                const geos::geom::Point* point = dynamic_cast<const geos::geom::Point*>(points->getGeometryN(i));
                if ( !point){
                    ERROR2(ERR_NO_INITIALIZED_2, "points", fcov->name());
                    return;
                }
                writePoint(point,output_file, raw);
            }
        }
        ++raw;
    }
}

bool FeatureConnector::storeBinaryData(FeatureCoverage *fcov, bool isMulti,IlwisTypes type) {
    if ( type == 0)
        return true;

    QString baseName = Ilwis3Connector::outputNameFor(fcov, isMulti, type);
    if (!storeBinaryDataTable(fcov, type, baseName)) // attribute tables
        return false;

    bool ok = true;
    if ( hasType(type, itPOLYGON)) {
        ok &= storeBinaryDataPolygon(fcov, baseName);
    } else if ( hasType(type, itLINE)) {
        ok &= storeBinaryDataLine(fcov, baseName);
    } else if ( hasType(type, itPOINT)){
        ok &= storeBinaryDataPoints(fcov, baseName);
    }
    return ok;
}

bool FeatureConnector::storeBinaryData(IlwisObject *obj) {
    FeatureCoverage *fcov = static_cast<FeatureCoverage *>(obj);
    IlwisTypes featureTypes = fcov->featureTypes();
    bool ok = true;
    bool isMulti = (featureTypes & (featureTypes - 1)) != 0 ; //(n & (n - 1)) != 0

    ok &= storeBinaryData(fcov, isMulti, featureTypes & itPOLYGON);
    ok &= storeBinaryData(fcov, isMulti, featureTypes & itLINE);
    ok &= storeBinaryData(fcov, isMulti, featureTypes & itPOINT);

    return ok;
}

void FeatureConnector::writeCoords(std::ofstream& output_file, const std::vector<geos::geom::Coordinate>* coords, bool singleton) {
    quint32 crdCount = coords->size();
    if(!singleton) {
        output_file.write((char *)&crdCount,4);
    }
    std::vector<double> crds(crdCount * 3);
    quint32 count = 0;
    for(const geos::geom::Coordinate& crd : *coords) {
        crds[count] = crd.x;
        crds[count+1] = crd.y;
        crds[count+2] = 0;
        count +=3;
    }
    output_file.write((char *)&crds[0], crdCount * 8 * 3);

}

void FeatureConnector::storeColumn(const QString& colName, const QString& domName, const QString& domInfo, const QString& storeType) {
    _odf->setKeyValue(colName, "Time", Time::now().toString());
    _odf->setKeyValue(colName, "Version", "3.1");
    _odf->setKeyValue(colName, "Class", "Column");
    _odf->setKeyValue(colName, "Domain", domName);
   _odf->setKeyValue(colName, "DomainInfo", domInfo);
    _odf->setKeyValue(colName, "StoreType", storeType);
     _odf->setKeyValue(colName, "Stored", "Yes");
    _odf->setKeyValue(colName, "ReadOnly", "No");
    _odf->setKeyValue(colName, "OwnedByTable", "Yes");
    _odf->setKeyValue(colName, "Type", "ColumnStore");
}

bool FeatureConnector::storeMetaLine(FeatureCoverage *fcov, const QString& filepath){
    QString dataFile = QFileInfo(filepath).fileName();

    _odf->setKeyValue("BaseMap","Type","SegmentMap");
    _odf->setKeyValue("SegmentMap","Type","SegmentMapStore");
    _odf->setKeyValue("SegmentMapStore","Format",QString::number(2));
    _odf->setKeyValue("SegmentMapStore","DeletedPolygons",QString::number(0));
    _odf->setKeyValue("Ilwis","Class","ILWIS::Segment Map");

    int noOfLines = fcov->featureCount(itLINE);
    _odf->setKeyValue("SegmentMapStore", "Segments", QString::number(noOfLines));

    _odf->setKeyValue("Table", "Domain", "None.dom");
    _odf->setKeyValue("Table", "DomainInfo", "None.dom;Byte;none;0;;");
    _odf->setKeyValue("Table", "Columns", QString::number(5));
    _odf->setKeyValue("Table", "Records", QString::number(noOfLines));
    _odf->setKeyValue("Table", "Type", "TableStore");

    _odf->setKeyValue("TableStore", "Data", dataFile + ".mps#");
    _odf->setKeyValue("TableStore", "UseAs", "No");
    _odf->setKeyValue("TableStore", "Type", "TableBinary");
    _odf->setKeyValue("TableStore", "Col0", "MinCoords");
    _odf->setKeyValue("TableStore", "Col1", "MaxCoords");
    _odf->setKeyValue("TableStore", "Col2", "Coords");
    _odf->setKeyValue("TableStore", "Col3", "Deleted");
    _odf->setKeyValue("TableStore", "Col4", "SegmentValue");

    storeColumn("Col:MinCoords","eth.csy","eth.csy;Coord;coord;0;;", "Coord");
    storeColumn("Col:MaxCoords","eth.csy","eth.csy;Coord;coord;0;;", "Coord");
    storeColumn("Col:Coords","CoordBuf.dom","CoordBuf.dom;?;coordbuf;0;;", "CoordBuf");
    storeColumn("Col:Deleted","bool.dom","bool.dom;Byte;bool;0;;", "Long");
    storeColumn("Col:SegmentValue",_domainName,_domainInfo, "Long");

    return true;
}

bool FeatureConnector::storeMetaPoint(FeatureCoverage *fcov, const QString& filepath){
    QString dataFile = QFileInfo(filepath).fileName();

    _odf->setKeyValue("BaseMap","Type","PointMap");
    _odf->setKeyValue("PointMap","Type","PointMapStore");
    _odf->setKeyValue("PointMapStore","Format",QString::number(2));
    _odf->setKeyValue("Ilwis","Class","ILWIS::Point Map");

    int noOfPoints = fcov->featureCount(itPOINT);
    _odf->setKeyValue("PointMap", "Points", QString::number(noOfPoints));

    _odf->setKeyValue("Table", "Domain", "None.dom");
    _odf->setKeyValue("Table", "DomainInfo", "None.dom;Byte;none;0;;");
    _odf->setKeyValue("Table", "Columns", QString::number(2));
    _odf->setKeyValue("Table", "Records", QString::number(noOfPoints));
    _odf->setKeyValue("Table", "Type", "TableStore");

    _odf->setKeyValue("TableStore", "Data", dataFile + ".pt#");
    _odf->setKeyValue("TableStore", "UseAs", "No");
    _odf->setKeyValue("TableStore", "Type", "TableBinary");
    _odf->setKeyValue("TableStore", "Col0", "Coordinate");
    _odf->setKeyValue("TableStore", "Col1", "Name");

    storeColumn("Col:Coordinate",_csyName,_csyName + ";Coord;coord;0;;","Coord");
    storeColumn("Col:Name",_domainName,_domainInfo, "Long");

    return true;
}

bool FeatureConnector::storeMetaPolygon(FeatureCoverage *fcov, const QString& filepath){
    QString dataFile = QFileInfo(filepath).fileName();

    _odf->setKeyValue("BaseMap","Type","PolygonMap");
    _odf->setKeyValue("PolygonMap","Type","PolygonMapStore");
    _odf->setKeyValue("PolygonMap","Toplogical","No");
    _odf->setKeyValue("PolygonMapStore","Format",QString::number(5));
    _odf->setKeyValue("PolygonMapStore","DeletedPolygons",QString::number(0));
    _odf->setKeyValue("Ilwis","Class","ILWIS::Polygon Map");

    QString localFile = dataFile + ".mpz#";
    _odf->setKeyValue("PolygonMapStore","DataPol", localFile);

    _odf->setKeyValue("PolygonMapStore", "Polygons", QString::number(countPolygons(fcov)));

    return true;
}

quint32  FeatureConnector::countPolygons(FeatureCoverage *fcov){
    quint32 count = 0;
    IFeatureCoverage features(fcov) ;
    for(const auto& feature : features){
        if ( feature->geometry().get() != 0){
            const UPGeometry& geom = feature->geometry();
            geos::geom::GeometryTypeId geostype = geom->getGeometryTypeId();
            if ( geostype == geos::geom::GEOS_POLYGON ){
                ++count;
            }else {
                const geos::geom::Geometry *polygons = geom.get();
                count += polygons->getNumGeometries();}
        }
    }

    return count;
}

bool FeatureConnector::storeMetaData(FeatureCoverage *fcov, IlwisTypes type) {
    if ( type == itUNKNOWN)
        return true;//if type is itUNKNOWN we dont store
    DataDefinition datadef;

    ITable attTable = fcov->attributeTable();
    int index = attTable->columnIndex(COVERAGEKEYCOLUMN);
    if ( index != iUNDEF ) {
        const ColumnDefinition& coldef = attTable->columndefinitionRef(index);
        if ( coldef.datadef().domain<>()->ilwisType() == itITEMDOMAIN)
            datadef = DataDefinition(coldef.datadef().domain(),coldef.datadef().range()->clone());
    } else {
        INamedIdDomain indexdom;
        indexdom.prepare();
        indexdom->name(fcov->name());
        NamedIdentifierRange range;
        for(int i=0; i < fcov->featureCount(type); ++i){
            QStringList parts = Ilwis3Connector::ilwis3ClassName(type).split(" ");
            QString itemname = QString("%1_%2").arg(parts[0]).arg(i);
            range << itemname;
        }
        indexdom->setRange(range);
        datadef.domain(indexdom);
        QFileInfo inf ( _resource.url().toLocalFile());
        QString filename = context()->workingCatalog()->filesystemLocation().toLocalFile() + "/" + inf.baseName() + ".dom";
        indexdom->connectTo(filename,"domain","ilwis3", Ilwis::IlwisObject::cmOUTPUT);
        indexdom->store();
    }
    bool isMulti = (fcov->featureTypes() & (fcov->featureTypes() - 1)) != 0;
    QString baseName = Ilwis3Connector::outputNameFor(fcov, isMulti, type);
    index = baseName.lastIndexOf(".");
    if ( index != -1) {
        baseName = baseName.left(index);
    }

    bool ok = CoverageConnector::storeMetaData(fcov, type, datadef.domain(), baseName);
    if ( !ok)
        return false;



    if ( datadef.domain()->valueType() == itINDEXEDITEM) {
        _odf->setKeyValue("Domain","Type","DomainUniqueID");
        _odf->setKeyValue("DomainSort","Sorting","AlphaNumeric");
        _odf->setKeyValue("DomainSort","Prefix","feature");
        _odf->setKeyValue("DomainSort","Class","Domain UniqueID");
        _odf->setKeyValue("DomainIdentifier","Nr",QString::number(fcov->featureCount(type)));
    }

    QString ext = "mpa";
    if ( hasType(type, itPOLYGON)){
        ok = storeMetaPolygon(fcov, baseName);
    }
    if ( hasType(type, itLINE)){
        ok = storeMetaLine(fcov, baseName);
        ext = "mps";
    }
    if ( hasType(type, itPOINT)){
        ok = storeMetaPoint(fcov, baseName);
        ext = "mpp";
    }
    if ( attTable.isValid() && attTable->columnCount() > 0) {
        QScopedPointer<TableConnector> conn(createTableStoreConnector(attTable, fcov, type,baseName));
        std::vector<quint32> recs(_itemCount);
        conn->selectedRecords(recs);
        conn->storeMetaData(attTable.ptr());
    }

    _odf->store(ext, QFileInfo(baseName));
    return ok;
}

bool FeatureConnector::storeMetaData(IlwisObject *obj)
{
    FeatureCoverage *fcov = static_cast<FeatureCoverage *>(obj);
    IlwisTypes featureTypes = fcov->featureTypes();
    bool ok = true;
    ok &= storeMetaData(fcov, featureTypes & itPOLYGON);
    ok &= storeMetaData(fcov, featureTypes & itLINE);
    ok &= storeMetaData(fcov, featureTypes & itPOINT);

    return ok;


}

bool FeatureConnector::storeBinaryDataTable(IlwisObject *obj, IlwisTypes tp, const QString& baseName)
{
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);
    ITable attTable = fcoverage->attributeTable();
    if ( attTable.isValid() && attTable->columnCount() > 0) {
        QScopedPointer<TableConnector> conn(createTableStoreConnector(attTable, fcoverage, tp, baseName));
        IFeatureCoverage cov(fcoverage);
        FeatureIterator iter(cov);
        quint32 i = 0;
        std::vector<quint32> recordnr(fcoverage->featureCount(tp));
        for(quint32 rec=0; rec < fcoverage->featureCount(); ++rec){
            if ( hasType((*iter)->geometryType(), tp))
                recordnr[i++] = rec;
            ++iter;
        };
        conn->selectedRecords(recordnr);
        return conn->storeBinaryData(attTable.ptr());

    }

    return true; // no store needed
}

QString FeatureConnector::format() const
{
    return "vectormap";
}
