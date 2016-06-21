#include "kernel.h"
#include "ilwisdata.h"
#include "version.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "factory.h"
#include "abstractfactory.h"
#include "domain.h"
#include "datadefinition.h"
#include "versioneddatastreamfactory.h"
#include "ilwisobjectconnector.h"
#include "streamconnector.h"
//#include "streamcoveragedatainterfacev1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer::VersionedSerializer(QDataStream& stream) : _stream(stream)
{
}

VersionedSerializer::~VersionedSerializer()
{

}

bool VersionedSerializer::loadItems(std::vector<Resource> &){
    QString var;
    _stream >> var;
    _stream >> var;
    _stream >> var;
    bool readonly;
    _stream >> readonly;
    double time;
    _stream >> time;
    _stream >> time;

    return true;
}

bool VersionedSerializer::dataIsLoaded() const
{
    return _dataLoaded;
}

bool VersionedSerializer::loadMetaData(IlwisObject *obj, const IOOptions &)
{
    if ( obj->isReadOnly())
        return false;
    bool alreadyThere =  obj->resource().isValid();

    // we are not going to replace info in the resource if not needed.
    // reason is that this can be a container object (e.g multiband raster) and we are reading here the container
    // and not the actual object embedded in the container
    QString var;
    _stream >> var;
    if (!alreadyThere)
        obj->name(var);
    _stream >> var;
    if (!alreadyThere)
        obj->code(var);
    _stream >> var;
    if (!alreadyThere)
        obj->setDescription(var);
    bool readonly;
     _stream >> readonly;
    if (!alreadyThere)
        obj->readOnly(readonly);
    double time;
    _stream >> time;
    if (!alreadyThere)
        obj->modifiedTime(time);
    _stream >> time;
    if (!alreadyThere)
        obj->createTime(time);

    return true;

}

bool VersionedSerializer::store(IlwisObject *obj, const IOOptions &options)
{
    double mtime = (double)obj->modifiedTime();
    double ctime = (double)obj->createTime();
    _stream <<  obj->ilwisType() << Version::interfaceVersion << obj->name() << obj->code() << obj->description() << obj->isReadOnly() << mtime << ctime;

    return true;
}

void VersionedSerializer::connector(StreamConnector *streamconnector)
{
    _streamconnector = streamconnector;
}

bool VersionedSerializer::storeDataDefintion(const DataDefinition &def, QDataStream &stream, const IOOptions &options) const
{
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::interfaceVersion, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;
    _stream << def.domain()->valueType();
    domainStreamer->store(def.domain().ptr(), options);
    if ( !def.range().isNull()) // no range for textdomains
        def.range()->store(_stream);

    return true;
}

bool VersionedSerializer::loadDataDefinition(DataDefinition &def, QDataStream &stream,const IOOptions &options)
{
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    IlwisTypes valueType, type;
    QString version;
    _stream >> valueType;
    _stream >> type;
    _stream >> version;
    std::unique_ptr<DataInterface> domainStreamer(factory->create(version, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;

    IDomain dom(type | valueType);
    Range *range = 0;
    domainStreamer->loadMetaData(dom.ptr(), options);
    if ( type != itTEXTDOMAIN){
        range = Range::create(dom->valueType());
        if (!range)
            return false;
        range->load(_stream);
    }
    def = DataDefinition(dom,range);

    return true;
}
