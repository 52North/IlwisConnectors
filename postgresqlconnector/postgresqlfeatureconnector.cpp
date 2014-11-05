#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "geometryhelper.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "domain.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "range.h"
#include "identifierrange.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"

#include "postgresqlconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqltableloader.h"
#include "postgresqldatabaseutil.h"
#include "postgresqlfeatureconnector.h"
#include "postgresqlfeaturecoverageloader.h"
#include "sqlstatementhelper.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlFeatureConnector::PostgresqlFeatureConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
}

PostgresqlFeatureConnector::~PostgresqlFeatureConnector()
{
}

IlwisObject *PostgresqlFeatureConnector::create() const
{
    //qDebug() << "PostgresqlTableConnector::create() -> FeatureCoverage";
    return new FeatureCoverage(_resource);
}

ConnectorInterface *PostgresqlFeatureConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlFeatureConnector(resource, load,options);
}

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const IOOptions &)
{
    //qDebug() << "PostgresqlFeatureConnector::loadMetaData()";

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    PostgresqlFeatureCoverageLoader loader(source());
    if ( !loader.loadMetadata(fcoverage)) {
        ERROR1(ERR_NO_INITIALIZED_1, source().name());
        return false;
    }

    return true;
}

bool PostgresqlFeatureConnector::loadData(IlwisObject *data, const IOOptions& options)
{
    ////qDebug() << "PostgresqlFeatureConnector::loadData()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    PostgresqlFeatureCoverageLoader loader(source());
    bool ok = loader.loadData(fcoverage);
    _binaryIsLoaded = ok;
    return ok;
}

bool PostgresqlFeatureConnector::store(IlwisObject *data, const IOOptions& options)
{
    //qDebug() << "PostgresqlFeatureConnector::store()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    IFeatureCoverage fcoverage;
    fcoverage.set(static_cast<FeatureCoverage *>(data));

    ITable baseData = fcoverage->attributeTable();
    PostgresqlTableConnector connector(_resource,false);
    bool tableOk = connector.store(baseData.ptr(), options);

    ITable geomData; // insert/update data
    IDomain semantics; // subfeature semantics
    QList<QString> primaryKeys; // readonly keys
    QList<MetaGeometryColumn> metaGeomColumns; // geometry columns
    PostgresqlDatabaseUtil::getMetaForGeometryColumns(_resource, metaGeomColumns);
    PostgresqlDatabaseUtil::prepareSubFeatureSementics(_resource, semantics);
    PostgresqlDatabaseUtil::getPrimaryKeys(_resource,primaryKeys);
    geomData.prepare();

    foreach (QString primaryKey, primaryKeys) {
        IDomain keyDomain;
        // TODO will text always work?
        keyDomain.prepare("primaryKey",itTEXTDOMAIN);
        geomData->addColumn(primaryKey, keyDomain);
    }

    quint32 depth = 0;
    if ( !semantics.isValid()) {
        // first come first serve case
        depth = metaGeomColumns.size();
        for (MetaGeometryColumn meta : metaGeomColumns) {
            IDomain wktDomain;
            QString geomColumn = meta.geomColumn;
            wktDomain.prepare("wkttext",itTEXTDOMAIN);
            geomData->addColumn(geomColumn, wktDomain);
        }
    } else {
        // order defines priority semantics
        depth = semantics->range<>()->count();
        ItemRangeIterator rangeIter(semantics->range<>().data());
        while (rangeIter.isValid()) {
            IDomain wktDomain;
            QString geomColumn = (*rangeIter)->name();
            wktDomain.prepare("wkttext",itTEXTDOMAIN);
            geomData->addColumn(geomColumn, wktDomain);
            ++rangeIter;
        }
    }

    // add geoms to update/insert data table
    FeatureIterator featureIter(fcoverage);
    featureIter.flow(FeatureIterator::fDEPTHFIRST);
    quint32 rowIdx = 0;
    while(featureIter != featureIter.end()) {
        // set primary key from level 0
        SPFeatureI feature = (*featureIter);
        for (quint32 keyIdx = 0 ; keyIdx < primaryKeys.size() ; keyIdx++) {
            Record record = feature->record();
            quint32 idx = baseData->columnIndex(primaryKeys[keyIdx]);
            QString value = record.cell(idx).toString();
            geomData->setCell(primaryKeys[keyIdx], rowIdx, value);
        }

        // add (sub)geometries and increase iterator
        quint32 geomIdxOffset = primaryKeys.size();
        for (quint32 level = 0 ; level < depth; level++) {

            /*
            // TODO storing level n data only makes sense
            //      if subfeatures are stored in separate
            //      tables related via foreign keys, but
            //      separate tables aren't supported now

            ITabel levelData = fcoverage->attributeTable(level);
            PostgresqlTableConnector connector(_resource,false);
            tableOk = connector.store(levelData.ptr(), options);
            */

            QString wkt = GeometryHelper::toWKT(feature->geometry().get());
            geomData->setCell(geomIdxOffset + level, rowIdx, wkt);

            ++featureIter; // next geom level
            feature = *(featureIter);
        }
        rowIdx++; // next row
    }




    SqlStatementHelper sqlHelper(_resource);
    sqlHelper.addCreateTempTableStmt("geom_data_level0");

/*

    QString sqlBuilder;
    sqlBuilder.append("UPDATE ");
    sqlBuilder.append(PostgresqlDatabaseUtil::qTableFromTableResource(_resource));
    sqlBuilder.append(" AS current");
    sqlBuilder.append(" SET ");

    for (MetaGeometryColumn column : metaGeomColunms) {
        sqlBuilder.append(column.geomColumn);
        sqlBuilder.append(" = updates.").append(column.geomColumn);
        sqlBuilder.append(", ");
    }

    sqlBuilder = trimAndRemoveLastCharacter(sqlBuilder);
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" ( VALUES ");

    while (featureIter != featureIter.end()) {
        sqlBuilder.append(" ( ");
        for (int i = 0 ; i < depth ; i++) {
            SPFeatureI feature = (*featureIter);

            // TODO respect isChanged() once implemented

            geos::geom::Geometry* geom = feature->geometry().get();
            sqlBuilder.append(" '");
            sqlBuilder.append(GeometryHelper::toWKT(geom));
            sqlBuilder.append("' , ");
            featureIter++;
        }
        sqlBuilder = trimAndRemoveLastCharacter(sqlBuilder);
        sqlBuilder.append("), ");
    }

    sqlBuilder = trimAndRemoveLastCharacter(sqlBuilder);
    sqlBuilder.append(" ) AS updates ( ");

    if (semantics.isValid()) {
        // respect defined order
        ItemRangeIterator rangeIter(semantics->range<>().data());
        while (rangeIter.isValid()) {
            sqlBuilder.append((*rangeIter)->name());
            sqlBuilder.append(", ");
            ++rangeIter;
        }
    } else {
        // respect first come first serve order
        foreach (MetaGeometryColumn column, metaGeomColunms) {
            sqlBuilder.append(column.geomColumn);
            sqlBuilder.append(", ");
        }
    }

    sqlBuilder = trimAndRemoveLastCharacter(sqlBuilder);
    sqlBuilder.append(" ) ");

    QString whereClause;
    QList<QString> primaryKeys;
    PostgresqlDatabaseUtil::getPrimaryKeys(_resource,primaryKeys);
    for (QString primaryKey : primaryKeys) {
        if (whereClause.isEmpty()) {
            whereClause.append(" WHERE ");
        } else {
            whereClause.append(" AND ");
        }
        whereClause.append(" current.").append(primaryKey);
        whereClause.append(" = ");
        whereClause.append(" updates.").append(primaryKey);
    }
    sqlBuilder.append(whereClause).append(" ;");


 */
    qDebug() << "SQL: " << sqlHelper.sql();

    QSqlDatabase db = PostgresqlDatabaseUtil::openForResource(_resource,"updategeometries");
    db.exec(sqlHelper.sql());
    db.close();

    bool featuresOk = true;
    return tableOk && featuresOk;
}


QString PostgresqlFeatureConnector::trimAndRemoveLastCharacter(const QString &string) {
    return string.left(string.trimmed().length() - 1);
}
