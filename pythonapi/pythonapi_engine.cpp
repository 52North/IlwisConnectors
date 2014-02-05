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

#include "../../IlwisCore/core/util/box.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"

#include "pythonapi_object.h"
#include "pythonapi_engine.h"
#include "pythonapi_pyvariant.h"
#include "pythonapi_rastercoverage.h"
#include "pythonapi_featurecoverage.h"
#include "pythonapi_pycontainer.h"

using namespace pythonapi;

Engine::Engine(){
}

Object *Engine::_do(std::string output_name, std::string operation, std::string c3, std::string c4, std::string c5,std::string c6, std::string c7, std::string c8, std::string c9){
    Ilwis::SymbolTable symtbl;
    Ilwis::ExecutionContext ctx;
    ctx.clear();
    //is no internal result name is given it will look like operation_id
    //but the id is to be added afterwards
    bool rename = false;
    if (output_name.empty()){
        output_name = operation;
        rename = true;
    }
    QString command;
    if (!c3.empty()){
        if(!c4.empty()){
            if(!c5.empty()){
                if(!c6.empty()){
                    if(!c7.empty()){
                        if(!c8.empty()){
                            if(!c9.empty()){
                                command = QString("script %1=%2(%3,%4,%5,%6,%7,%8,%9)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str(),c8.c_str(),c9.c_str());
                            }else{
                                command = QString("script %1=%2(%3,%4,%5,%6,%7,%8)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str(),c8.c_str());
                            }
                        }else{
                            command = QString("script %1=%2(%3,%4,%5,%6,%7)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str());
                        }
                    }else{
                        command = QString("script %1=%2(%3,%4,%5,%6)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str());
                    }
                }else{
                    command = QString("script %1=%2(%3,%4,%5)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str());
                }
            }else{
                command = QString("script %1=%2(%3,%4)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str());
            }
        }else{
            command = QString("script %1=%2(%3)").arg(output_name.c_str(),operation.c_str(),c3.c_str());
        }
    }else{
        command = QString("script %1=%2").arg(output_name.c_str(),operation.c_str());
    }
    if (Ilwis::commandhandler()->execute(command,&ctx, symtbl) && !ctx._results.empty()){
        Ilwis::Symbol result = symtbl.getSymbol(ctx._results[0]);
        if (result._type == itRASTER){
            if (result._var.canConvert<Ilwis::IRasterCoverage>()){
                Ilwis::IRasterCoverage* obj = new Ilwis::IRasterCoverage(result._var.value<Ilwis::IRasterCoverage>());
                if (rename)
                    (*obj)->setName(QString("%1_%2").arg(operation.c_str()).arg((*obj)->id()));
                return new RasterCoverage(obj);
            }
        }else if (result._type == itFEATURE){
            if (result._var.canConvert<Ilwis::IFeatureCoverage>()){
                Ilwis::IFeatureCoverage* obj = new Ilwis::IFeatureCoverage(result._var.value<Ilwis::IFeatureCoverage>());
                if (rename)
                    (*obj)->setName(QString("%1_%2").arg(operation.c_str()).arg((*obj)->id()));
                return new FeatureCoverage(obj);
            }
        }else if (result._type == itCOORDSYSTEM){
            if (result._var.canConvert<Ilwis::ICoordinateSystem>()){
                Ilwis::ICoordinateSystem* obj = new Ilwis::ICoordinateSystem(result._var.value<Ilwis::ICoordinateSystem>());
                if (rename)
                    (*obj)->setName(QString("%1_%2").arg(operation.c_str()).arg((*obj)->id()));
                return new CoordinateSystem(obj);
            }
        }
        return new PyVariant(new QVariant(result._var));
    }else{
        throw Ilwis::ErrorObject(QString("couln't do(%1)").arg(command));
    }
}

bool Engine::setWorkingCatalog(const std::string& location){
    Ilwis::Catalog cat;
    cat.prepare(QString::fromStdString(location), QString("container='%1'").arg(location.c_str()));
    if(cat.isValid()){
        Ilwis::context()->setWorkingCatalog(cat);
        return true;
    }else
        throw Ilwis::ErrorObject(QString("invalid container location: '%1'").arg(location.c_str()));
}


PyObject* Engine::operations(const std::string& filter){
    Ilwis::Catalog opCat;
    opCat.prepare(QUrl(QString("ilwis://operations")), "type='OperationMetaData'");
    std::list<Ilwis::Resource> ops = opCat.items();
    PyObject* list = newPyTuple(ops.size());
    int i = 0;
    for(auto it = ops.begin(); it != ops.end(); it++){
        if (!setTupleItem(list, i++, it->name().toStdString().data())){
            throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of attributes").arg( it->name()));
        }
    }
    return list;
}

std::string Engine::operationMetaData(const std::string &name){
    Ilwis::Catalog opCat;
    opCat.prepare(QUrl(QString("ilwis://operations")), "type='OperationMetaData'");
    std::list<Ilwis::Resource> ops = opCat.items();
    QString ret;
    for(auto it = ops.begin();it != ops.end(); it++){
        if (QString::fromStdString(name).compare(it->name(),Qt::CaseInsensitive) == 0){
            if(!ret.isEmpty())
                ret.append("\n");
            ret.append((*it)["syntax"].toString());
        }
    }
    return ret.toStdString();
}
