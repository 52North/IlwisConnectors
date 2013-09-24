
#include "kernel.h"
#include "connectorinterface.h"
#include "domainitem.h"
#include "ilwisdata.h"
#include "domain.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "numericrange.h"
#include "range.h"
#include "itemrange.h"
#include "identifierrange.h"
#include "ilwisobjectconnector.h"
#include "gdalproxy.h"
#include "gdalconnector.h"
#include "numericdomain.h"
#include "domainconnector.h"


using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *DomainConnector::create(const Resource& resource, bool load) {
    return new DomainConnector(resource);

}

DomainConnector::DomainConnector(const Resource& resource, bool load) : GdalConnector(resource,load)
{
}

bool DomainConnector::loadMetaData(IlwisObject *data){
    if (!GdalConnector::loadMetaData(data))
        return false;


    if (type() == itUNKNOWN) {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Domain type",data->name()));
        return false;
    }
    bool ok = false;

    if ( type() == itNUMERICDOMAIN) {
        ok = handleValueDomains(data);
    } else if ( type() == itITEMDOMAIN) {
        ok = handleThematicDomains(data);
    }

    gdal()->closeFile(_filename, data->id());
    return ok;

}

bool DomainConnector::handleThematicDomains(IlwisObject* ) {
    //TODO handle thematic raster domains
    return false;
}

bool DomainConnector::handleValueDomains(IlwisObject* data) {
    NumericDomain *vdata = static_cast<NumericDomain*>(data);

    return true;
}

IlwisObject *DomainConnector::create() const
{
    //TODO other domain types time, identifier
    //TODO what about other system domain types that are value derived.
    if ( type() == itNUMERICDOMAIN)
        return new NumericDomain(_resource);
    //else if ( type() == itTHEMATICDOMAIN) {
    //    return new ItemDomain<ThematicItem>(_resource);
    //}
    return 0;
}
