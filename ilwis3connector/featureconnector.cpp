#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "coverage.h"
#include "module.h"
#include "inifile.h"
#include "ilwisdata.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "table.h"
#include "geometry.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "containerstatistics.h"
#include "rawconverter.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "binaryilwis3table.h"
#include "coverageconnector.h"
#include "featureconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *FeatureConnector::create(const Resource &item, bool load) {
    return new FeatureConnector(item, load);

}

Ilwis::IlwisObject *FeatureConnector::create() const
{
    return new Ilwis::FeatureCoverage(_resource);
}

void FeatureConnector::calcStatics(const IlwisObject *obj, CoverageStatistics::PropertySets set) const
{
}

FeatureConnector::FeatureConnector(const Resource &item, bool load) : CoverageConnector(item, load)
{
}

bool FeatureConnector::loadBinaryPolygons30(FeatureCoverage *fcoverage, ITable& tbl) {
    return false;
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
    int nrPolygons = fcoverage->featureCount(itPOLYGONCOVERAGE);
    SPAttributeRecord record( new AttributeRecord(tbl,"coverage_key"));
    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;

    for(int j=0; j < nrPolygons; ++j) {
        Polygon pol;
        readRing(stream, pol.outer());
        double value;
        quint32 numberOfHoles;
        stream >> value;
        stream >> numberOfHoles;
        pol.inners().resize(numberOfHoles);
        for(int i=0; i< numberOfHoles;++i)
            readRing(stream, pol.inners()[i]);
        if ( isNumeric) {
            tbl->cell("coverage_key", j, QVariant(j));
            tbl->cell("feature_value", j, QVariant(value));
            fcoverage->newFeature({pol},j, record);
        } else {
            quint32 itemId = value;
            tbl->cell("coverage_key", j, QVariant(itemId));
            fcoverage->newFeature({pol},itemId, record);
        }

    }

    return true;
}

bool FeatureConnector::readRing(QDataStream& stream, std::vector<Coordinate2d> &ring ) {
    quint32 numberOfCoords;

    if (stream.readRawData((char *)&numberOfCoords, 4) <= 0)
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,"data file");
    Coordinate *p = new Coordinate[numberOfCoords];
    stream.readRawData((char *)p,numberOfCoords*3*8);
    ring.resize(numberOfCoords);
    std::copy(p, p + numberOfCoords, ring.begin());

   return true;
}

bool FeatureConnector::loadBinaryPolygons(FeatureCoverage *fcoverage, ITable& tbl) {
    QString dataFile = _odf->value("PolygonMapStore","DataPol");
    if ( dataFile != sUNDEF) {
        return loadBinaryPolygons30(fcoverage, tbl);
    } else {
        return loadBinaryPolygons37(fcoverage, tbl);
    }
    return false;
}

bool FeatureConnector::loadBinarySegments(FeatureCoverage *fcoverage, ITable& tbl) {
    BinaryIlwis3Table mpsTable;
    if ( !mpsTable.load(_odf)) {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->fileinfo().fileName())    ;
    }
    int colCoords = mpsTable.index("Coords");
    int colItemId = mpsTable.index("SegmentValue");
    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;
    if ( isNumeric) // in other case nr of record already has been set as it is based on a real table
        tbl->setRows(mpsTable.rows());

    SPAttributeRecord record( new AttributeRecord(tbl,"coverage_key"));
    double value;
    for(quint32 i= 0; i < mpsTable.rows(); ++i) {
        std::vector<Coordinate > coords;
        mpsTable.get(i,colCoords,coords);
        Line2D<Coordinate2d > line;
        line.resize(coords.size());
        std::copy(coords.begin(), coords.end(), line.begin());
        mpsTable.get(i, colItemId,value);
        if ( isNumeric) {
            tbl->cell("coverage_key", i, QVariant(i));
            tbl->cell("feature_value", i, QVariant(value));
            fcoverage->newFeature({line},i, record);
        } else {
            quint32 itemId = value;
            tbl->cell("coverage_key", i, QVariant(itemId));
            fcoverage->newFeature({line},itemId, record);
        }


    }
    return true;


}

bool FeatureConnector::loadBinaryPoints(FeatureCoverage *fcoverage, ITable& tbl) {
    BinaryIlwis3Table mppTable;
    if ( !mppTable.load(_odf)) {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1,_odf->fileinfo().fileName())    ;
    }
    // two cases; the old case; 2 columns for x and y. and the new case one column for coord
    int coordColumnX = mppTable.index("x");
    int coordColumnY = mppTable.index("y");
    int coordColumn = mppTable.index("Coordinate");
    int colItemId = mppTable.index("Name");

    bool newCase =  coordColumnX == iUNDEF;
    SPAttributeRecord record( new AttributeRecord(tbl,"coverage_key"));


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
        tbl->cell("coverage_key", i, QVariant(itemId));

        fcoverage->newFeature({c},itemId, record);

    }
    return true;
}

bool FeatureConnector::loadBinaryData(Ilwis::IlwisObject *obj) {
    if ( obj == nullptr)
        return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);

    IDomain covdom;
    ITable tbl = fcoverage->attributeTable(ilwisType(_odf->fileinfo().fileName()));
    if (!covdom.prepare("count")){
        return false;
    }
    tbl->addColumn("coverage_key",covdom);

    bool isNumeric = _odf->value("BaseMap","Range") != sUNDEF;
    if ( isNumeric) {
        IDomain featuredom;
        if (!featuredom.prepare("value")){
            return false;
        }
        tbl->addColumn("feature_value",featuredom);
    }

    if (fcoverage->featureTypes() == itPOINTCOVERAGE)
        return loadBinaryPoints(fcoverage, tbl);
    else if (fcoverage->featureTypes() == itSEGMENTCOVERAGE)
        return loadBinarySegments(fcoverage, tbl);
    else if (fcoverage->featureTypes() == itPOLYGONCOVERAGE)
        return loadBinaryPolygons(fcoverage, tbl);
    return false;
}

bool FeatureConnector::loadMetaData(Ilwis::IlwisObject *obj)
{
    bool ok = CoverageConnector::loadMetaData(obj);
    if ( !ok)
        return false;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);
    IlwisTypes coverageType = ilwisType(_odf->fileinfo().fileName());
    fcoverage->featureTypes(coverageType);


    int features = _odf->value("PointMap","Points").toInt(&ok);
    if (!ok) {
        features = _odf->value("SegmentMapStore","Segments").toInt(&ok);
        if (!ok) {
            features = _odf->value("PolygonMapStore","Polygons").toInt(&ok);
        }
    }
    if (ok)
        fcoverage->setFeatureCount(coverageType, features);
    else
       return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Records",obj->name());

    return true;

}

bool FeatureConnector::storeMetaData(IlwisObject *)
{
    return false;
}
