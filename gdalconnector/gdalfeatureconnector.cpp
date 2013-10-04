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
#include "identifieritem.h"
#include "identifierrange.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"
#include "coverageconnector.h"
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

IlwisTypes GdalFeatureConnector::getFeatureType(OGRLayerH hLayer) const{
    IlwisTypes ret = itUNKNOWN;
    OGRwkbGeometryType type = gdal()->getLayerGeometry(hLayer);
    if ( type == wkbPoint || type == wkbMultiPoint || type == wkbPoint25D || type == wkbMultiPoint25D)
        ret += itPOINT;

    if ( type == wkbLineString || type == wkbMultiLineString || type == wkbLineString25D || type == wkbMultiLineString25D)
        ret += itLINE;

    if ( type == wkbPolygon || type == wkbMultiPolygon || type == wkbPolygon25D || type == wkbMultiPolygon25D)
        ret += itPOLYGON;

    return ret;
}


bool GdalFeatureConnector::loadMetaData(Ilwis::IlwisObject *data){

    if(!GdalConnector::loadMetaData(data))
        return false;

    OGRSFDriverH driver;
    _dataSource = gdal()->openOGRFile(_filename, data->id(), GA_ReadOnly, &driver);
    if (!_dataSource){
        return ERROR2(ERR_COULD_NOT_OPEN_READING_2,_filename,QString(gdal()->getLastErrorMsg()));
    }
    QFileInfo inf(_filename);//TODO: what about replacing QString _filename by a QFileInfo
    data->setName(inf.fileName());

    if(!CoverageConnector::loadMetaData(data))
        return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IlwisTypes coverageType = itUNKNOWN;
    int featureCount = 0;

    int layerCount = gdal()->getLayerCount(_dataSource);
    for(int layer = 0; layer < layerCount ; ++layer) {
        OGRLayerH hLayer = gdal()->getLayer(_dataSource, layer);
        if ( hLayer) {
            IlwisTypes type = getFeatureType(hLayer);
            if (type == itUNKNOWN){
                ERROR2(ERR_COULD_NOT_LOAD_2,QString("Layer %1 from:").arg(_filename),QString(":%1").arg(gdal()->getLastErrorMsg()));
            }
            coverageType |= type;
            featureCount += gdal()->getFeatureCount(_dataSource, FALSE) ? -1 : 0;//TRUE to FORCE databases to scan whole layer, FALSe can end up in -1 for unknown result
        }
    }
    if (coverageType != itUNKNOWN && featureCount > 0){
        fcoverage->featureTypes(coverageType);
        fcoverage->setFeatureCount(coverageType, featureCount);
    }else
       return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Records",data->name());

    ITable tbl = fcoverage->attributeTable();
    tbl->setRows(fcoverage->featureCount());

    gdal()->closeFile(_filename, data->id()); //TODO: when to close a file
    return true;
}

bool GdalFeatureConnector::store(IlwisObject *obj, IlwisTypes type)
{
    return CoverageConnector::store(obj, type);
}

void GdalFeatureConnector::reportError(OGRDataSourceH dataSource) const
{
    kernel()->issues()->log(QString(gdal()->getLastErrorMsg()));
    gdal()->releaseDataSource(dataSource);
}

