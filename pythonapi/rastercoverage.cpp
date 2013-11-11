#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/raster.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/rastercoverage.h"

#include "rastercoverage.h"
#include "engine.h"

using namespace pythonapi;

RasterCoverage::RasterCoverage(){
    Ilwis::IRasterCoverage fc;
    fc.prepare();
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

RasterCoverage::RasterCoverage(const char *resource){
    Ilwis::IRasterCoverage fc;
    fc.prepare(QString(resource));
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

RasterCoverage* RasterCoverage::__add__(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(QString("result = %1 + %2").arg((*this->ptr())->name(), (*rc.ptr())->name()).toLocal8Bit());
}

double RasterCoverage::value(double x, double y, double z){
    return this->ptr()->get<Ilwis::RasterCoverage>()->pix2value(Coordinate(x,y,z));
}

RasterCoverage* RasterCoverage::toRasterCoverage(Object* obj){
    RasterCoverage* ptr = static_cast<RasterCoverage*>(obj);
    if(!ptr)
        throw Ilwis::ErrorObject(QString("cast to RasterCoverage not possible"));
    return ptr;
}
