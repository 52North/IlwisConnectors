#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "coverage.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "coverageserializerv1.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"
#include "featureserializerv1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *FeatureSerializerV1::create(QDataStream& stream)
{
    return new FeatureSerializerV1(stream);
}

FeatureSerializerV1::FeatureSerializerV1(QDataStream &stream) : CoverageSerializerV1(stream)
{
}

bool FeatureSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    if (!CoverageSerializerV1::store(obj, options))
        return false;

    IFeatureCoverage fcoverage;
    fcoverage.prepare(obj->id());

    _stream << fcoverage->featureCount();
    for(const UPFeatureI& feature : fcoverage){
        feature->store(_stream);
    }

    return true;
}

bool FeatureSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!CoverageSerializerV1::loadMetaData(obj, options))
        return false;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);
    quint32 featureCount;
    _stream >> featureCount;
    for(quint32 f = 0; f < featureCount; ++f){
        UPFeatureI& feature = fcoverage->newFeature(0, false); // create an empty feature
        feature->load(_stream, fcoverage->geomfactory());
    }
    return true;
}
