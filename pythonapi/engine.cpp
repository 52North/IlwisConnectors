#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operationmetadata.h"
#include "../../IlwisCore/core/ilwisobjects/operation/symboltable.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operationExpression.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operation.h"
#include "../../IlwisCore/core/ilwisobjects/operation/commandhandler.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/raster.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/rastercoverage.h"

#include "../../IlwisCore/core/util/angle.h"
#include "../../IlwisCore/core/util/point.h"
#include "../../IlwisCore/core/util/line.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/polygon.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/geometry.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"

#include "engine.h"
#include "pyvariant.h"

using namespace pythonapi;

Engine::Engine(){
}

Object *Engine::_do(const char *command){
    Ilwis::SymbolTable symtbl;
    Ilwis::ExecutionContext ctx;
    ctx.clear();
    Ilwis::commandhandler()->execute(QString("result=rainfall.tbt[march]"),&ctx, symtbl);
    Ilwis::Symbol result = symtbl.getSymbol(ctx._results[0]);
    if (result._type == itRASTER){
        if (result._var.canConvert<Ilwis::IRasterCoverage>())
            return new IlwisObject(new Ilwis::IIlwisObject(result._var.value<Ilwis::IRasterCoverage>()));
    }else if (result._type == itFEATURE){
        if (result._var.canConvert<Ilwis::IFeatureCoverage>())
            return new IlwisObject(new Ilwis::IIlwisObject(result._var.value<Ilwis::IFeatureCoverage>()));
    }
    return new PyVariant(new QVariant(result._var));
}