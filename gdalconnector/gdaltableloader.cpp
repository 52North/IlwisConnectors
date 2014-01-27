#include "kernel.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "datadefinition.h"
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
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "gdalconnector.h"
#include "juliantime.h"
#include "gdaltableloader.h"

using namespace Ilwis;
using namespace Gdal;

GdalTableLoader::GdalTableLoader()
{
}

GdalTableLoader::~GdalTableLoader()
{
    for(FillerColumnDef *def : _columnDefinitions)
        delete def;
}

void GdalTableLoader::loadMetaData(Table *attTable, OGRLayerH hLayer) {
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
        ColumnDefinition colDef(name, domain,i+1);
        attTable->addColumn(colDef);
    }

}

void GdalTableLoader::loadRecord(Table *attTable, OGRFeatureH hFeature, std::vector<QVariant>& record ) {
    for (int i = 0; i < attTable->columnCount() -2;i++){
        if (_columnDefinitions[i]){
            record[i] = (this->*_columnDefinitions[i]->fillFunc)(hFeature, _columnDefinitions[i]->index);
        }
    }
}

void GdalTableLoader::setColumnCallbacks(Table * attTable, OGRLayerH hLayer){
    OGRFeatureDefnH hLayerDef = gdal()->getLayerDef(hLayer);
    int gdalFieldCount = gdal()->getFieldCount(hLayerDef);
    _columnDefinitions.resize(attTable->columnCount() - 2,0);
    int offset = 0;
    for (int i = 0; i < attTable->columnCount() - 2;i++){
        if (i+offset < gdalFieldCount){//check if coulumn was added to metadata
            OGRFieldDefnH hFieldDefn = gdal()->getFieldDfn(hLayerDef, i+offset);
            QString name = QString(gdal()->getFieldName(hFieldDefn));
            ColumnDefinition& coldef = attTable->columndefinition(i);
            if (coldef.name().compare(name,Qt::CaseSensitive) == 0){//check if column was deleted from metadata
                DataDefinition& datadef = coldef.datadef();
                if(datadef.domain().isValid()){
                    IlwisTypes tp = datadef.domain()->valueType();
                    if (tp & itSTRING){
                        _columnDefinitions[i] = new FillerColumnDef(&GdalTableLoader::fillStringColumn, i+offset);
                    }else if (tp & itINTEGER){
                        NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
                        //creating the actual range as invalid to be adjusted in the fillers
                        double min = r->min();
                        r->min(r->max());
                        r->max(min);
                        datadef.range(r);
                        _columnDefinitions[i] = new FillerColumnDef(&GdalTableLoader::fillIntegerColumn, i+offset);
                    }else if (tp & itDOUBLE){
                        //creating the actual range as invalid to be adjusted in the fillers
                        NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
                        double min = r->min();
                        r->min(r->max());
                        r->max(min);
                        datadef.range(r);
                        _columnDefinitions[i] = new FillerColumnDef(&GdalTableLoader::fillDoubleColumn, i+offset);
                    }else if (tp & itTIME){
                        //creating the actual range as invalid to be adjusted in the fillers
                        NumericRange* r = static_cast<NumericRange*>(datadef.domain()->range2range<NumericRange>()->clone());
                        double min = r->min();
                        r->min(r->max());
                        r->max(min);
                        datadef.range(r);
                        _columnDefinitions[i] = new FillerColumnDef(&GdalTableLoader::fillDateTimeColumn, i+offset);
                    }else{
                        _columnDefinitions[i] = nullptr;
                    }
                }else{
                    _columnDefinitions[i] = nullptr;
                }
            }else{//column was deleted from metadata
                _columnDefinitions[i] = new FillerColumnDef(&GdalTableLoader::fillEmptyColumn, -1);
                offset++;
            }
        }else{//coulumn was added to metadata
            _columnDefinitions[i] = new FillerColumnDef(&GdalTableLoader::fillEmptyColumn, -1);
        }

    }
}

QVariant GdalTableLoader::fillEmptyColumn(OGRFeatureH , int ){
    return QVariant();
}

QVariant GdalTableLoader::fillStringColumn(OGRFeatureH featureH, int colIntex){
    return QVariant(gdal()->getFieldAsString(featureH, colIntex));
}

QVariant GdalTableLoader::fillIntegerColumn(OGRFeatureH featureH, int colIntex){
    int v = gdal()->getFieldAsInt(featureH, colIntex);
    return QVariant(v);
}
//TODO:: not yet tested
QVariant GdalTableLoader::fillDoubleColumn(OGRFeatureH featureH, int colIntex){
    double v = gdal()->getFieldAsDouble(featureH, colIntex);
    return QVariant(v);
}
//TODO:: not yet tested
QVariant GdalTableLoader::fillDateTimeColumn(OGRFeatureH featureH, int colIntex){
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

