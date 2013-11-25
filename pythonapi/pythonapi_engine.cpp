#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwiscontext.h"
#include "../../IlwisCore/core/catalog/catalog.h"

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

#include "pythonapi_object.h"
#include "pythonapi_engine.h"
#include "pythonapi_pyvariant.h"

using namespace pythonapi;

Engine::Engine(){
}

Object *Engine::_do(const char* output_name, const char* operation, const char *c3, const char *c4, const char *c5,const char* c6, const char* c7, const char* c8, const char* c9){
    Ilwis::SymbolTable symtbl;
    Ilwis::ExecutionContext ctx;
    ctx.clear();
    //is no internal result name is given it will look like operation_id
    //but the id is to be added afterwards
    bool rename = false;
    if (QString(output_name).isEmpty()){
        output_name = operation;
        rename = true;
    }
    QString command;
    if (!QString(c3).isEmpty()){
        if(!QString(c4).isEmpty()){
            if(!QString(c5).isEmpty()){
                if(!QString(c6).isEmpty()){
                    if(!QString(c7).isEmpty()){
                        if(!QString(c8).isEmpty()){
                            if(!QString(c9).isEmpty()){
                                command = QString("script %1=%2(%3,%4,%5,%6,%7,%8,%9)").arg(output_name,operation,c3,c4,c5,c6,c7,c8,c9);
                            }else{
                                command = QString("script %1=%2(%3,%4,%5,%6,%7,%8)").arg(output_name,operation,c3,c4,c5,c6,c7,c8);
                            }
                        }else{
                            command = QString("script %1=%2(%3,%4,%5,%6,%7)").arg(output_name,operation,c3,c4,c5,c6,c7);
                        }
                    }else{
                        command = QString("script %1=%2(%3,%4,%5,%6)").arg(output_name,operation,c3,c4,c5,c6);
                    }
                }else{
                    command = QString("script %1=%2(%3,%4,%5)").arg(output_name,operation,c3,c4,c5);
                }
            }else{
                command = QString("script %1=%2(%3,%4)").arg(output_name,operation,c3,c4);
            }
        }else{
            command = QString("script %1=%2(%3)").arg(output_name,operation,c3);
        }
    }else{
        command = QString("script %1=%2").arg(output_name,operation);
    }
    if (Ilwis::commandhandler()->execute(command,&ctx, symtbl) && !ctx._results.empty()){
        Ilwis::Symbol result = symtbl.getSymbol(ctx._results[0]);
        if (result._type == itRASTER){
            if (result._var.canConvert<Ilwis::IRasterCoverage>()){
                Ilwis::IIlwisObject* obj = new Ilwis::IIlwisObject(result._var.value<Ilwis::IRasterCoverage>());
                if (rename)
                    (*obj)->setName(QString("%1_%2").arg(operation).arg((*obj)->id()));
                return new IlwisObject(obj);
            }
        }else if (result._type == itFEATURE){
            if (result._var.canConvert<Ilwis::IFeatureCoverage>()){
                Ilwis::IIlwisObject* obj = new Ilwis::IIlwisObject(result._var.value<Ilwis::IFeatureCoverage>());
                if (rename)
                    (*obj)->setName(QString("%1_%2").arg(operation).arg((*obj)->id()));
                return new IlwisObject(obj);
            }
        }
        return new PyVariant(new QVariant(result._var));
    }else{
        throw Ilwis::ErrorObject(QString("couln't do(%1)").arg(command));
    }
}

bool Engine::setWorkingCatalog(const char *location){
    Ilwis::Catalog cat;
    cat.prepare(QString(location), QString("container='%1'").arg(location));
    if(cat.isValid()){
        Ilwis::context()->setWorkingCatalog(cat);
        return true;
    }else
        throw Ilwis::ErrorObject(QString("invalid container location: '%1'").arg(location));
}
