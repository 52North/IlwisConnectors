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
#include "connectorinterface.h"
#include "containerconnector.h"
#include "inifile.h"
#include "coverage.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "table.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "containerstatistics.h"
#include "rawconverter.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "binaryilwis3table.h"
#include "coordinatedomain.h"
#include "coverageconnector.h"
#include "ilwis3featureconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

struct XYZ {
    double x,y,z;
};

ConnectorInterface *FeatureConnector::create(const Resource &resource, bool load) {
    return new FeatureConnector(resource, load);

}

Ilwis::IlwisObject *FeatureConnector::create() const
{
    return new Ilwis::FeatureCoverage(_resource);
}

void FeatureConnector::calcStatics(const IlwisObject *obj, NumericStatistics::PropertySets set) const
{
}

FeatureConnector::FeatureConnector(const Resource &resource, bool load) : CoverageConnector(resource, load)
{
}

bool FeatureConnector::loadBinaryPolygons30(FeatureCoverage *fcoverage, ITable& tbl) {
    BinaryIlwis3Table polTable;
    if ( !polTable.load(_odf)) {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->file())    ;
    }

    BinaryIlwis3Table topTable;
    if ( !topTable.load(_odf,"top")) {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->file())    ;
    }

    qint32 colValue = polTable.index("PolygonValue");
    qint32 colTopStart = polTable.index("TopStart");
    qint32 colArea = polTable.index("Area");
    int nrPolygons = polTable.rows();
    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;

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
            for(int i=1; i < rings->size(); ++i) {
                (*geoms)[i-1] = rings->at(i);
            }

            geos::geom::Polygon *polygon = fcoverage->geomfactory()->createPolygon(outer, geoms);
            //std::copy(rings[0].begin(), rings[0].end(), polygon.outer().begin());
            polTable.get(i, colValue, v);
            if ( isNumeric) {
                tbl->setCell(COVERAGEKEYCOLUMN, i, QVariant(i));
                tbl->setCell(FEATUREVALUECOLUMN, i, QVariant(v));
                fcoverage->newFeature({polygon}, false);
            } else {
                quint32 itemId = v;
                tbl->setCell(COVERAGEKEYCOLUMN, i, QVariant(itemId - 1));
                fcoverage->newFeature({polygon}, false);
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
        topTable.get(row,colCoords,coords);
        if( coords.size() == 0 ||coords.back() == coords.front()){
            ring = new std::vector<geos::geom::Coordinate>(coords.size());
            std::copy(coords.begin(), coords.end(), ring->begin());
        } else if ( coords.back() == ring->back()) {
            ring = new std::vector<geos::geom::Coordinate>(coords.size());
            std::reverse_copy(coords.begin(), coords.end(), ring->begin());
        } else if ( ring->front() == coords.front()) {
            ring = new std::vector<geos::geom::Coordinate>(coords.size());
            std::reverse_copy(coords.begin(), coords.end(), ring->begin());
        } else if ( ring->front() == coords.back()) {
            ring = new std::vector<geos::geom::Coordinate>(coords.size());
            std::copy(coords.begin(), coords.end(), ring->begin());
        }

        if ( ring->front() == ring->back() && ring->size() > 3) {
            geos::geom::CoordinateArraySequence * ringIn = new geos::geom::CoordinateArraySequence(ring);
            ringIn->removeRepeatedPoints();
            rings->push_back( fcoverage->geomfactory()->createLinearRing(ringIn));
            coords.clear();
        }
        qint32 oldIndex = row;
        double v;
        if ( forward)
            topTable.get(abs(row),colForward,v);
        else
            topTable.get(abs(row), colBackward, v);\
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
    if ( fwl != iUNDEF)
        --fwl; // due to being raw values
    topTable.get(abs(index),colBackward, v );
    bwl = v;
    if ( bwl != iUNDEF)
        --bwl;

    if ( abs(fwl) == abs(bwl)	)
        return true;
    if ( index < 0)
        return false;
    std::vector<Coordinate> startLine, forwardLine;
    topTable.get(abs(index), colCoords,startLine);
    topTable.get(abs(fwl), colCoords, forwardLine);

    bool forward = false;
    if ( fwl > 0)
        forward =  startLine.back() == forwardLine.front();
    else
        forward = startLine.back() == forwardLine.back();

    return forward;

}


bool FeatureConnector::loadBinaryPolygons37(FeatureCoverage *fcoverage, ITable& tbl) {
    QString datafile = _odf->value("PolygonMapStore","DataPol");
    datafile = context()->workingCatalog()->filesystemLocation().toLocalFile() + "/" + datafile;
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
    fcoverage->setFeatureCount(itPOLYGON, 0,0); // metadata already set it to correct number, creating new features will up the count agains; so reset to 0.
    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;

    for(int j=0; j < nrPolygons; ++j) {
        geos::geom::CoordinateArraySequence *outer = readRing(stream);
        if ( !outer)
            return false;

        geos::geom::LinearRing *outerring = fcoverage->geomfactory()->createLinearRing(outer);
        double value;
        quint32 numberOfHoles;
        stream.readRawData((char *)&value, 8);
        stream.readRawData((char *)&numberOfHoles, 4);
        std::vector<geos::geom::Geometry*> *inners = new std::vector<geos::geom::Geometry*>(numberOfHoles);
        for(quint32 i=0; i< numberOfHoles;++i)
            (*inners)[i] = fcoverage->geomfactory()->createLinearRing(readRing(stream));
        geos::geom::Polygon *pol = fcoverage->geomfactory()->createPolygon(outerring, inners);
        if ( isNumeric) {

            fcoverage->newFeature({pol},false);
            tbl->setCell(COVERAGEKEYCOLUMN, j, QVariant(j));
            tbl->setCell(FEATUREVALUECOLUMN, j, QVariant(value));
        } else {
            quint32 itemId = value;
            fcoverage->newFeature({pol},false);
            tbl->setCell(COVERAGEKEYCOLUMN, j, QVariant(itemId - 1));
        }

    }
    file.close();

    return true;
}

geos::geom::CoordinateArraySequence* FeatureConnector::readRing(QDataStream& stream ) {
    quint32 numberOfCoords;

    if (stream.readRawData((char *)&numberOfCoords, 4) <= 0){
        ERROR1(ERR_COULD_NOT_OPEN_READING_1,"data file");
        return 0;
    }
    vector<XYZ> pnts(numberOfCoords);
    stream.readRawData((char *)&pnts[0],numberOfCoords*3*8);
    geos::geom::CoordinateArraySequence *ring = new geos::geom::CoordinateArraySequence(numberOfCoords);
    for(quint32 i=0; i < numberOfCoords; ++i) {
        ring->setAt(geos::geom::Coordinate(pnts[i].x, pnts[i].y, pnts[i].z), i);
    }

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
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->file())    ;
    }
    int colCoords = mpsTable.index("Coords");
    int colItemId = mpsTable.index("SegmentValue");
    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;
    ITable tbl = fcoverage->attributeTable();
    fcoverage->setFeatureCount(itLINE, 0,0); // metadata already set it to correct number, creating new features will up the count agains; so reset to 0.

    double value;
    for(quint32 i= 0; i < mpsTable.rows(); ++i) {
        std::vector<Coordinate > coords;
        mpsTable.get(i,colCoords,coords);
        geos::geom::CoordinateArraySequence *vertices = new geos::geom::CoordinateArraySequence(coords.size());
        for(int i=0; i < coords.size(); ++i)
            vertices->setAt(geos::geom::Coordinate(coords[i].x, coords[i].y, coords[i].z), i);
        geos::geom::LineString *line = fcoverage->geomfactory()->createLineString(vertices);
        mpsTable.get(i, colItemId,value);
        if ( isNumeric) {
            fcoverage->newFeature({line},false);
            tbl->setCell(COVERAGEKEYCOLUMN, i, QVariant(i));
            tbl->setCell(FEATUREVALUECOLUMN, i, QVariant(value));

        } else {
            quint32 itemId = value;
            tbl->setCell(COVERAGEKEYCOLUMN, i, QVariant(itemId - 1));
            fcoverage->newFeature({line},false);
        }


    }
    return true;


}

bool FeatureConnector::loadBinaryPoints(FeatureCoverage *fcoverage) {
    BinaryIlwis3Table mppTable;
    if ( !mppTable.load(_odf)) {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->file())    ;
    }
    // two cases; the old case; 2 columns for x and y. and the new case one column for coord
    int coordColumnX = mppTable.index("x");
    int coordColumnY = mppTable.index("y");
    int coordColumn = mppTable.index("Coordinate");
    int colItemId = mppTable.index("Name");

    ITable tbl = fcoverage->attributeTable();
    bool newCase =  coordColumnX == iUNDEF;
    fcoverage->setFeatureCount(itPOINT, 0,0); // metadata already set it to correct number, creating new features will up the count agains; so reset to 0.

    for(quint32 i= 0; i < mppTable.rows(); ++i) {
        Coordinate c;
        double itemIdT;
        if ( newCase) {
            mppTable.get(i, coordColumn, c);
        } else {
            double x,y;
            mppTable.get(i, coordColumnX, x);
            mppTable.get(i, coordColumnY, y);
            c = Coordinate(x,y);
        }
        mppTable.get(i, colItemId,itemIdT);
        quint32 itemId = itemIdT;
        geos::geom::Point *point = fcoverage->geomfactory()->createPoint(c);
        fcoverage->newFeature(point, false);
        tbl->setCell(COVERAGEKEYCOLUMN, i, QVariant(itemId - 1));

    }
    return true;
}

bool FeatureConnector::loadBinaryData(Ilwis::IlwisObject *obj) {
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
        if (fcoverage->featureTypes() == itPOINT)
            ok = loadBinaryPoints(fcoverage);
        else if (fcoverage->featureTypes() == itLINE)
            ok = loadBinarySegments(fcoverage);
        else if (fcoverage->featureTypes() == itPOLYGON)
            ok = loadBinaryPolygons(fcoverage);

        if ( ok && extTable.isValid()) {
            ITable attTbl = fcoverage->attributeTable();
            //quint32 keyIndex = attTbl->columnIndex(COVERAGEKEYCOLUMN);
            for(quint32 rowExt=0; rowExt < extTable->recordCount(); ++rowExt) {
                vector<QVariant> rec = extTable->record(rowExt);
                attTbl->record(rowExt,rec);
            }
        }
    } catch (FeatureCreationError& ) {
    }
    if ( ok)
        _binaryIsLoaded = true;
    return ok;
}

bool FeatureConnector::loadMetaData(Ilwis::IlwisObject *obj)
{
    bool ok = CoverageConnector::loadMetaData(obj);
    if ( !ok)
        return false;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);
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

    ITable tbl = fcoverage->attributeTable();
    tbl->recordCount(fcoverage->featureCount(itFEATURE,true));
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
    for_each(iter, iter.end(), [&](UPFeatureI& feature){
        const UPGeometry& geom = feature->geometry();
        for(int i=0; i < feature->trackSize(); ++i) {
            geos::geom::GeometryTypeId geostype = geom->getGeometryTypeId();
            if ( geostype == geos::geom::GEOS_POLYGON || geostype == geos::geom::GEOS_MULTIPOLYGON) {
                const geos::geom::Geometry *polygons = geom.get();
                if ( geostype == geos::geom::GEOS_POLYGON ){
                    const geos::geom::Polygon *polygon = dynamic_cast<const geos::geom::Polygon*>(polygons);
                    if ( !polygon)
                        return ERROR2(ERR_NO_INITIALIZED_2, "polygon", fcov->name());
                    writePolygon(polygon,output_file,raw);
                } else {
                    int n = polygons->getNumGeometries();
                    for(int i = 0; i < n ; ++i){
                        const geos::geom::Polygon* polygon = dynamic_cast<const geos::geom::Polygon*>(polygons->getGeometryN(i));
                        if ( !polygon)
                            return ERROR2(ERR_NO_INITIALIZED_2, "polygon", fcov->name());
                        writePolygon(polygon,output_file,raw);
                    }
                }

                ++raw;
            }
        }

    });

    output_file.close();

    return true;
}


void FeatureConnector::writeLine(const geos::geom::LineString* line,std::ofstream& output_file,double raw ) {
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
    output_file.write((char *)&raw, 8);
}

bool FeatureConnector::storeBinaryDataLine(FeatureCoverage *fcov, const QString& baseName) {
    std::ofstream output_file;
    QFileInfo inf(baseName);
    QString dir = context()->workingCatalog()->location().toLocalFile();
    QString filename = dir + "/" + inf.baseName() + ".mps#";
    output_file.open(filename.toLatin1(),ios_base::out | ios_base::binary | ios_base::trunc);
    if ( !output_file.is_open())
        return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,filename);
    char header[128];
    memset(header, 0, 128);
    output_file.write(header,128);

    IFeatureCoverage cov;
    cov.set(fcov);
    FeatureIterator iter(cov);
    double raw = 1;

    for_each(iter, iter.end(), [&](UPFeatureI& feature){
        const UPGeometry& geom = feature->geometry();
        for(int i=0; i < feature->trackSize(); ++i) {
            geos::geom::GeometryTypeId geostype = geom->getGeometryTypeId();
            if ( geostype == geos::geom::GEOS_MULTILINESTRING || geos::geom::GEOS_LINESTRING) {

                const geos::geom::Geometry *lines = geom.get();
                if ( geostype == geos::geom::GEOS_LINESTRING ){
                    const geos::geom::LineString *line = dynamic_cast<const geos::geom::LineString*>(lines);
                    if ( !line)
                        return ERROR2(ERR_NO_INITIALIZED_2, "lines", fcov->name());
                    writeLine(line,output_file,raw);
                } else {
                    int n = lines->getNumGeometries();
                    for(int i = 0; i < n ; ++i){
                        const geos::geom::LineString* line = dynamic_cast<const geos::geom::LineString*>(lines->getGeometryN(i));
                        if ( !line)
                            return ERROR2(ERR_NO_INITIALIZED_2, "lines", fcov->name());
                        writeLine(line,output_file,raw);
                    }
                }
                ++raw;
            }
        }

    });

    output_file.close();

    return true;
}

void FeatureConnector::writePoint(const geos::geom::Point* point,std::ofstream& output_file,long raw ) {
    writeCoord(output_file, *(point->getCoordinate()));
    output_file.write((char *)&raw, 4);
}

bool FeatureConnector::storeBinaryDataPoints(FeatureCoverage *fcov, const QString& baseName) {
    std::ofstream output_file;
    QFileInfo inf(baseName);
    QString dir = context()->workingCatalog()->location().toLocalFile();
    QString filename = dir + "/" + inf.baseName() + ".pt#";
    output_file.open(filename.toLatin1(),ios_base::out | ios_base::binary | ios_base::trunc);
    if ( !output_file.is_open())
        return ERROR1(ERR_COULD_NOT_OPEN_WRITING_1,filename);
    char header[128];
    memset(header, 0, 128);
    output_file.write(header,128);

    IFeatureCoverage cov;
    cov.set(fcov);
    FeatureIterator iter(cov);
    double raw = 1;

    for_each(iter, iter.end(), [&](UPFeatureI& feature){
        const UPGeometry& geom = feature->geometry();
        for(int i=0; i < feature->trackSize(); ++i) {
            geos::geom::GeometryTypeId geostype = geom->getGeometryTypeId();
            if ( geostype == geos::geom::GEOS_MULTIPOINT ||geostype == geos::geom::GEOS_POINT) {

                const geos::geom::Geometry *points = geom.get();
                if ( geostype == geos::geom::GEOS_POINT ){
                    const geos::geom::Point *point = dynamic_cast<const geos::geom::Point*>(points);
                    if ( !point)
                        return ERROR2(ERR_NO_INITIALIZED_2, "points", fcov->name());
                    writePoint(point,output_file, raw);
                } else {
                    int n = points->getNumGeometries();
                    for(int i = 0; i < n ; ++i){
                        const geos::geom::Point* point = dynamic_cast<const geos::geom::Point*>(points->getGeometryN(i));
                        if ( !point)
                            return ERROR2(ERR_NO_INITIALIZED_2, "points", fcov->name());
                        writePoint(point,output_file, raw);
                    }
                }
                ++raw;
            }
        }

    });

    output_file.close();

    return true;
}

bool FeatureConnector::storeBinaryData(FeatureCoverage *fcov, IlwisTypes type) {
    if ( type == 0)
        return true;
    if (!CoverageConnector::storeBinaryData(fcov, type)) // attribute tables
        return false;
    QString baseName = Ilwis3Connector::outputNameFor(fcov);
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

    ok &= storeBinaryData(fcov, featureTypes & itPOLYGON);
    ok &= storeBinaryData(fcov, featureTypes & itLINE);
    ok &= storeBinaryData(fcov, featureTypes & itPOINT);

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

    int noOfLines = fcov->featureCount(itLINE,true);
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

    int noOfPoints = fcov->featureCount(itPOINT, true);
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
    _odf->setKeyValue("PolygonMapStore", "Polygons", QString::number(fcov->featureCount(itPOLYGON, true)));

    return true;
}

bool FeatureConnector::storeMetaData(FeatureCoverage *fcov, IlwisTypes type) {
    if ( type == itUNKNOWN)
        return true;//if type is itUNKNOWN we dont store
    DataDefinition datadef;

    ITable attTable = fcov->attributeTable();
    ColumnDefinition coldef = attTable->columndefinition(COVERAGEKEYCOLUMN);
    if ( coldef.isValid()) {
        datadef = coldef.datadef();
    } else {
        INamedIdDomain indexdom;
        indexdom.prepare();
        indexdom->setName(fcov->name());
        NamedIdentifierRange range;
        for(int i=0; i < fcov->featureCount(type); ++i){
            QStringList parts = Ilwis3Connector::ilwis3ClassName(type).split(" ");
            QString itemname = QString("%1_%2").arg(parts[0]).arg(i);
            range << itemname;
        }
        indexdom->setRange(range);
        datadef.domain(indexdom);
    }

    bool ok = CoverageConnector::storeMetaData(fcov, type, datadef.domain());
    if ( !ok)
        return false;

    QString dataFile = Ilwis3Connector::outputNameFor(fcov);
    int index = dataFile.lastIndexOf(".");
    if ( index != -1) {
        dataFile = dataFile.left(index);
    }

    if ( datadef.domain()->valueType() == itINDEXEDITEM) {
        _odf->setKeyValue("Domain","Type","DomainUniqueID");
        _odf->setKeyValue("DomainSort","Sorting","AlphaNumeric");
        _odf->setKeyValue("DomainSort","Prefix","feature");
        _odf->setKeyValue("DomainSort","Class","Domain UniqueID");
        _odf->setKeyValue("DomainIdentifier","Nr",QString::number(fcov->featureCount(type,true)));
    }

    QString ext = "mpa";
    if ( hasType(fcov->featureTypes(), itPOLYGON)){
        ok = storeMetaPolygon(fcov, dataFile);
    }
    if ( hasType(fcov->featureTypes(), itLINE)){
        ok = storeMetaLine(fcov, dataFile);
        ext = "mps";
    }
    if ( hasType(fcov->featureTypes(), itPOINT)){
        ok = storeMetaPoint(fcov, dataFile);
        ext = "mpp";
    }

    _odf->store(ext, containerConnector());
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

QString FeatureConnector::type2Prefix(IlwisTypes tp) {
    if ( tp == itPOINT)
        return "point";
    if (tp == itLINE)
        return "seg";
    if ( tp == itPOLYGON)
        return "pol";

    return "feature";
}
