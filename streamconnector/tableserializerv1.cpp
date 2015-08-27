#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "connectorinterface.h"
#include "tableserializerv1.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *TableSerializerV1::create(QDataStream& stream)
{
    return new TableSerializerV1(stream);
}

TableSerializerV1::TableSerializerV1(QDataStream& stream) : VersionedSerializer(stream)
{
}

bool TableSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Table *tbl = static_cast<Table *>(obj);
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    _stream << tbl->columnCount();
    _stream << tbl->recordCount();
    std::vector<IlwisTypes> types;
    for(int col = 0; col < tbl->columnCount(); ++col){
        const ColumnDefinition& coldef = tbl->columndefinitionRef(col);
        _stream << coldef.name();

        std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::IlwisVersion, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;
        domainStreamer->store(coldef.datadef().domain().ptr(), options);
        if ( !coldef.datadef().range().isNull()) // no range for textdomains
            coldef.datadef().range()->store(_stream);
        types.push_back(coldef.datadef().domain()->valueType());
    }
    for(int rec = 0; rec < tbl->recordCount(); ++rec){
        auto record = tbl->record(rec);
        record.storeData(types, _stream,options);
    }
    return true;
}

bool TableSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    Table *tbl = static_cast<Table *>(obj);
    int columnCount, recordCount;
    std::vector<IlwisTypes> types;
    QString version;
    quint64 type;
    _stream >> columnCount;
    _stream >> recordCount;
    for(int col =0; col < columnCount; ++col){
        QString columnName;
        _stream >> columnName;

        IlwisTypes valueType;
        _stream >> valueType;

        _stream >> type;
        _stream >> version;
        std::unique_ptr<DataInterface> domainStreamer(factory->create(version, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;

        IDomain dom(type | valueType);
        Range *range = 0;
        types.push_back(valueType);
        domainStreamer->loadMetaData(dom.ptr(), options);
        if ( type != itTEXTDOMAIN){
            range = Range::create(dom->valueType());
            if (!range)
                return false;
            range->load(_stream);
        }

        tbl->addColumn(ColumnDefinition(columnName, dom));
        if ( range)
            tbl->columndefinition(col).datadef().range(range);

    }

    for(quint32 rec = 0; rec < recordCount; ++rec){
        Record& record = tbl->newRecord();
        record.loadData(types,_stream,options);

    }
    return true;
}
