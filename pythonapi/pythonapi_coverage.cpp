#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"

#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"

#include "pythonapi_domain.h"
#include "pythonapi_coverage.h"
#include "pythonapi_pyobject.h"
#include "pythonapi_util.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_columndefinition.h"

using namespace pythonapi;

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


