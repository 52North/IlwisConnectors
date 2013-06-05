#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "angle.h"
#include "point.h"
#include "connectorinterface.h"
#include "inifile.h"
#include "errorobject.h"
#include "ilwisdata.h"
#include "domainitem.h"
#include "domain.h"
#include "itemdomain.h"
#include "numericrange.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "databasetable.h"
#include "numericdomain.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "tableconnector.h"
#include "binaryilwis3table.h"
#include "rawconverter.h"
#include "ilwis3range.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *TableConnector::create(const Resource &item, bool load) {
    return new TableConnector(item, load);

}

TableConnector::TableConnector(const Resource &item, bool load) : Ilwis3Connector(item, load)
{
   // _type = itTABLE;
}

bool TableConnector::loadMetaData(IlwisObject *data)
{
    if(!Ilwis3Connector::loadMetaData(data))
        return false;

    quint32 ncolumns = _odf->value("Table","Columns").toInt();
    quint32 rows = _odf->value("Table","Records").toInt();
    QVector<ColumnDefinition>  columns;
    ColumnDefinition key  = getKeyColumn();
    if ( key.isValid()) {
        columns.push_back(key);
    }

    Table *tbl = static_cast<Table *>(data);
    for(quint32 index=0; index < ncolumns; ++index) {
        QString colName = _odf->value("TableStore", QString("Col%1").arg(index));
        ColumnDefinition col = makeColumn(colName, index);
        tbl->addColumn(col);

    }
    if ( tbl->ilwisType() == itDATABASETABLE) {
        DatabaseTable *dbtable = static_cast<DatabaseTable *>(tbl);
        dbtable->setDatabase(kernel()->database());
    }
    tbl->setRows(rows);
    return true;

}

QString TableConnector::type() const
{
    return "Table";
}

IlwisObject *TableConnector::create() const
{
    return new FlatTable(_resource);
}

ColumnDefinition TableConnector::makeColumn(const QString& colName, quint64 index) {

    QString domName = _odf->value("Col:" + colName,"Domain");
    IDomain dom;
    if ( isSystemObject(domName)) {
        domName = noExt(domName);
    }
    if(!dom.prepare(domName)) {
        return ColumnDefinition();
    }
    QString section = QString("Col:%1").arg(colName);
    RawConverter conv = Ilwis3Range::converter(_odf,_odf->value(section,"Range" ));
    _converters[colName] = conv;
    ColumnDefinition col(colName, dom, index );
    return col;
}

ColumnDefinition TableConnector::getKeyColumn() {
    QString domain = _odf->value("Table","Domain");
    if ( domain == "None")
        return ColumnDefinition();
    IThematicDomain dom;
    dom.prepare(domain);
    if ( !dom.isValid()) {
        kernel()->issues()->log(TR(ERR_NO_INITIALIZED_1).arg(domain));
        return ColumnDefinition();
    }
    IDomain dmkey;
    dmkey.prepare("count");
    ColumnDefinition colKey(dom->name()+"_raw", dmkey, 0);
    colKey.datadef().range(new NumericRange(0,dom->count(),1));
    return colKey;

}

bool TableConnector::loadBinaryData(IlwisObject* data ) {
    Ilwis3::BinaryIlwis3Table tbl ;
    if (!tbl.load(_odf)) // no table found?
        return false;
    Table *table = static_cast<Table *>(data);

    if(!table->createTable())
        return false;

    for(quint32 i = 0; i < tbl.columns(); ++i) {
        QString colName = tbl.columnName(i);
        ColumnDefinition col = table->columndefinition(colName);
        if ( col.isValid()) {
            QVariantList varlist;
            RawConverter conv = _converters[colName];
            for(quint32 j = 0; j < tbl.rows(); ++j){
                if ( col.datadef().domain()->valueType() >= itINT8 && col.datadef().domain()->valueType() <= itDOUBLE) {
                    double value;
                    if (tbl.get(j,i,value)) {
                        varlist <<  conv.raw2real(value);
                    }
                } else if (col.datadef().domain()->valueType() == itSTRING) {
                    QString value;
                    if (tbl.get(j,i,value)) {
                        varlist << value;
                    }
                }
            }
            table->column(colName,varlist);
        } else {
            kernel()->issues()->log(TR(ERR_NO_OBJECT_TYPE_FOR_2).arg("column", colName));
            return false;
        }

    }


    return true;
}

QString TableConnector::valueType2DataType(IlwisTypes ty) {
    QString vType=sUNDEF;
    if ( (ty >= itINT8 && ty <= itINT64) || ty ==itITEMDOMAIN) {
        vType = "Integer"        ;
    } else if ( ty >= itINT8 && ty <= itDOUBLE) {
        vType = "Real";
    } else if ( ty == itSTRING) {
        vType = "Text";
    } else {
        //TODO other domain types
    }
    return vType;
}
