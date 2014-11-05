
#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "domain.h"
#include "juliantime.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"

#include "sqlstatementhelper.h"
#include "postgresqldatabaseutil.h"
#include "postgresqlconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqltableloader.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlTableConnector::PostgresqlTableConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
}

IlwisObject *PostgresqlTableConnector::create() const
{
    //qDebug() << "PostgresqlTableConnector::create() -> FlatTable";
    return new FlatTable(_resource);
}

ConnectorInterface *PostgresqlTableConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlTableConnector(resource, load, options);
}

bool PostgresqlTableConnector::loadMetaData(IlwisObject *data, const IOOptions& options)
{
    //qDebug() << "PostgresqlTableConnector::loadMetaData()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    Table *table = static_cast<Table *>(data);
    PostgresqlTableLoader loader(source());
    return loader.loadMetadata(table);
}

bool PostgresqlTableConnector::store(IlwisObject *data, const IOOptions& options)
{
    //qDebug() << "PostgresqlTableConnector::store()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    Table *table = static_cast<Table *>(data);
    SPSqlStatementHelper sqlHelper = SPSqlStatementHelper(new SqlStatementHelper(_resource));
    sqlHelper->addCreateTempTableStmt("data_level_0");
    sqlHelper->addInsertChangedDataToTempTableStmt("data_level_0", table);
    sqlHelper->addUpdateStmt("data_level_0", table);
    sqlHelper->addInsertStmt("data_level_0", table);

    // TODO delete deleted rows

    //qDebug() << "SQL: " << sqlHelper->sql();

    QSqlDatabase db = PostgresqlDatabaseUtil::openForResource(_resource,"upserting_table");
    db.exec(sqlHelper->sql());
    db.close();

    return true;
}

QString PostgresqlTableConnector::trimAndRemoveLastCharacter(const QString &string) {
    return string.left(string.trimmed().length() - 1);
}

QString PostgresqlTableConnector::createInsertValueString(QVariant value, const ColumnDefinition &coldef) const {
    IDomain domain = coldef.datadef().domain<>();
    if (hasType(domain->valueType(),itINTEGER)) {

        return QString::number(value.toInt());

    } else if (hasType(domain->valueType(),itDOUBLE | itFLOAT)) {

        return QString::number(value.toDouble());

    } else if (hasType(domain->valueType(),itTHEMATICITEM | itNAMEDITEM | itINDEXEDITEM | itNUMERICITEM | itTIMEITEM)) {

        return domain->impliedValue(value).toString();

    } else if (hasType(domain->valueType(), itDATETIME)) {
        if ( QString(value.typeName()).compare("Ilwis::Time") != 0){
            ERROR2(ERR_COULD_NOT_CONVERT_2,value.toString(), "time");
            Time time(0,0,0,0,0);
            return time.toString(itDATE);
        } else{
            Time time = value.value<Ilwis::Time>();
            return time.toString(itDATETIME);
        }
    } else if (hasType(domain->valueType(),itSTRING)){

        return QString("'%1'").arg(value.toString());

    } else {
        ERROR0("Could not determine data type.");
        return QString("");
    }
}

bool PostgresqlTableConnector::loadData(IlwisObject *data,const IOOptions& options)
{
    //qDebug() << "PostgresqlTableConnector::loadData()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    Table *table = static_cast<Table *>(data);
    PostgresqlTableLoader loader(source());
    if ( !loader.loadMetadata(table)) {
        return false;
    }
    bool ok = loader.loadData(table);
    _binaryIsLoaded = ok;
    return ok;
}

