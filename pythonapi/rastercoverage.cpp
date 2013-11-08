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

using namespace pythonapi;

RasterCoverage::RasterCoverage(){
    Ilwis::IRasterCoverage fc;
    fc.prepare();
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IlwisData<Ilwis::IlwisObject>(fc));
}

RasterCoverage::RasterCoverage(const char *resource){
    Ilwis::IRasterCoverage fc;
    fc.prepare(QString(resource));
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IlwisData<Ilwis::IlwisObject>(fc));
}

