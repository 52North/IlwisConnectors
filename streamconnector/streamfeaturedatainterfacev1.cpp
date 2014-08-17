#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "streamconnectorv1.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "coverage.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "streamcoveragedatainterfacev1.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"
#include "streamfeaturedatainterfacev1.h"

using namespace Ilwis;
using namespace Stream;

DataInterface *StreamFeatureDataInterfaceV1::create(QDataStream& stream)
{
    return new StreamFeatureDataInterfaceV1(stream);
}

StreamFeatureDataInterfaceV1::StreamFeatureDataInterfaceV1(QDataStream &stream) : StreamCoverageDataInterfaceV1(stream)
{
}

bool StreamFeatureDataInterfaceV1::store(IlwisObject *obj, int options)
{
    if (!StreamCoverageDataInterfaceV1::store(obj, options))
        return false;

    IFeatureCoverage fcoverage;
    fcoverage.prepare(obj->id());

    _stream << fcoverage->featureCount();
    for(const UPFeatureI& feature : fcoverage){
        feature->store(_stream);
    }

    return true;
}

bool StreamFeatureDataInterfaceV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!StreamCoverageDataInterfaceV1::loadMetaData(obj, options))
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
