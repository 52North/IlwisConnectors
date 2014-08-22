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
#include "streamconnectorv1.h"
#include "streamcoordinatesystemdatainterfacev1.h"
#include "streamdomaindatainterfacev1.h"
#include "streamcoveragedatainterfacev1.h"
#include "streamfeaturedatainterfacev1.h"
#include "streamtabledatainterfacev1.h"
#include "streamprojectiondatainterfacev1.h"
#include "streamellipsoiddatainterfacev1.h"
#include "streamgeorefdatainterfacev1.h"
#include "streamrasterdatainterfacev1.h"

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

    versionFactory->addCreator({"iv40",itFEATURE},StreamFeatureDataInterfaceV1::create);
    versionFactory->addCreator({"iv40",itDOMAIN},StreamDomainDataInterfaceV1::create);
    versionFactory->addCreator({"iv40",itTABLE},StreamTableDataInterfaceV1::create);
    versionFactory->addCreator({"iv40",itCOORDSYSTEM},StreamCoordinateSystemDataInterfaceV1::create);
    versionFactory->addCreator({"iv40",itELLIPSOID},StreamEllipsoidDataInterfaceV1::create);
    versionFactory->addCreator({"iv40",itPROJECTION},StreamProjectionDataInterfaceV1::create);
    versionFactory->addCreator({"iv40",itGEOREF},StreamGeorefDataInterfaceV1::create);
    versionFactory->addCreator({"iv40",itRASTER},StreamRasterDataInterfaceV1::create);


    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;

    cfactory->addCreator("featurecoverage","stream",StreamConnector::create);
    cfactory->addCreator("rastercoverage","stream",StreamConnector::create);



}
