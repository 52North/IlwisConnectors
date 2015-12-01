#include "kernel.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "streamobjectfactory.h"
#include "streamconnector.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
#include "streammodule.h"
#include "versioneddatastreamfactory.h"
#include "versionedserializer.h"
#include "coordinatesystemSerializerv1.h"
#include "domainSerializerv1.h"
#include "coverageSerializerv1.h"
#include "featureSerializerv1.h"
#include "tableSerializerv1.h"
#include "projectionSerializerv1.h"
#include "ellipsoidSerializerv1.h"
#include "georefSerializerv1.h"
#include "rasterSerializerv1.h"
#include "catalogserializerv1.h"
#include "operationmetadataserializerv1.h"
#include "workflowserializerv1.h"
#include "dataformat.h"

using namespace Ilwis;
using namespace Stream;

StreamModule::StreamModule(QObject *parent) : Module(parent, "StreamModule", "iv40","1.0")
{

}

StreamModule::~StreamModule()
{

}

QString StreamModule::getInterfaceVersion() const
{
      return "iv40";
}

QString StreamModule::getName() const
{
    return "stream plugin";
}

QString StreamModule::getVersion() const
{
    return "1.0";
}

void StreamModule::prepare()
{

    StreamObjectFactory *factory = new StreamObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );


    VersionedDataStreamFactory *versionFactory = new VersionedDataStreamFactory();
    kernel()->addFactory(versionFactory);

    versionFactory->addCreator({"iv40",itFEATURE},FeatureSerializerV1::create);
    versionFactory->addCreator({"iv40",itDOMAIN},DomainSerializerV1::create);
    versionFactory->addCreator({"iv40",itTABLE},TableSerializerV1::create);
    versionFactory->addCreator({"iv40",itCOORDSYSTEM},CoordinateSystemSerializerV1::create);
    versionFactory->addCreator({"iv40",itELLIPSOID},EllipsoidSerializerV1::create);
    versionFactory->addCreator({"iv40",itPROJECTION},ProjectionSerializerV1::create);
    versionFactory->addCreator({"iv40",itGEOREF},GeorefSerializerV1::create);
    versionFactory->addCreator({"iv40",itRASTER},RasterSerializerV1::create);
    versionFactory->addCreator({"iv40",itCATALOG},CatalogserializerV1::create);
    versionFactory->addCreator({"iv40",itOPERATIONMETADATA},OperationMetadataSerializerV1::create);
    versionFactory->addCreator({"iv40",itWORKFLOW},WorkflowSerializerV1::create);


    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;

    //inputconnectors
    cfactory->addCreator(itILWISOBJECT  ,"stream", StreamConnector::create);

    //outputconnectors
    cfactory->addCreator("featurecoverage","stream",StreamConnector::create);
    cfactory->addCreator("rastercoverage","stream",StreamConnector::create);
    cfactory->addCreator("table","stream",StreamConnector::create);
    cfactory->addCreator("catalog","stream",StreamConnector::create);
    cfactory->addCreator("domain","stream",StreamConnector::create);
    cfactory->addCreator("georeference","stream",StreamConnector::create);
    cfactory->addCreator("coordinatesystem","stream",StreamConnector::create);
    cfactory->addCreator("operationmetadata","stream",StreamConnector::create);
    cfactory->addCreator("workflow","stream",StreamConnector::create);

    QFileInfo ilw = context()->ilwisFolder();
    QString path = ilw.canonicalFilePath() + "/extensions/streamconnector/resources";
    DataFormat::setFormatInfo(path + "/streamformats.config","ilwis3");

    kernel()->issues()->log("Loaded streaming module",IssueObject::itMessage);



}
