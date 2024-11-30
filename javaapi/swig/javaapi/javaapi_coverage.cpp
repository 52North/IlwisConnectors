#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"

#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"

#include "javaapi_domain.h"
#include "javaapi_coverage.h"
#include "javaapi_util.h"
#include "javaapi_columndefinition.h"

using namespace javaapi;

Coverage::Coverage():IlwisObject(){}

Coverage::Coverage(Ilwis::ICoverage *coverage):IlwisObject(new Ilwis::IIlwisObject(*coverage)){
    delete coverage;
}

CoordinateSystem Coverage::coordinateSystem(){
    return CoordinateSystem(new Ilwis::ICoordinateSystem(this->ptr()->as<Ilwis::Coverage>()->coordinateSystem()));
}

Envelope Coverage::envelope(){
    return Envelope(this->ptr()->as<Ilwis::Coverage>()->envelope());
}

void Coverage::setEnvelope(const Envelope &env){
    this->ptr()->as<Ilwis::Coverage>()->envelope(env.data());
}


