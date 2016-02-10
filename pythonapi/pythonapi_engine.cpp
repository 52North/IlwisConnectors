#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwiscontext.h"
#include "../../IlwisCore/core/catalog/catalog.h"
#include "../../IlwisCore/core/catalog/catalogview.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operationmetadata.h"
#include "../../IlwisCore/core/ilwisobjects/operation/symboltable.h"
#include "../../IlwisCore/core/ilwisobjects/operation/commandhandler.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operationExpression.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operation.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributedefinition.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/raster.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/rastercoverage.h"

#include "../../IlwisCore/core/util/angle.h"

#include "../../IlwisCore/core/util/box.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"


#include "pythonapi_object.h"
#include "pythonapi_engine.h"
#include "pythonapi_rastercoverage.h"
#include "pythonapi_featurecoverage.h"
#include "pythonapi_pyobject.h"
#include "pythonapi_catalog.h"

using namespace pythonapi;

Engine::Engine(){
}

Object* Engine::_do(std::string output_name, std::string operation, std::string c3, std::string c4, std::string c5,std::string c6, std::string c7, std::string c8, std::string c9){
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
                            command = QString("%1=%2(%3,%4,%5,%6,%7)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str());
                        }
                    }else{
                        command = QString("%1=%2(%3,%4,%5,%6)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str());
                    }
                }else{
                    command = QString("%1=%2(%3,%4,%5)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str());
                }
            }else{
                command = QString("%1=%2(%3,%4)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str());
            }
        }else{
            command = QString("%1=%2(%3)").arg(output_name.c_str(),operation.c_str(),c3.c_str());
        }
    }else{
        command = QString("%1=%2").arg(output_name.c_str(),operation.c_str());
    }
    if (Ilwis::commandhandler()->execute(command,&ctx, symtbl) && !ctx._results.empty()){
        Ilwis::Symbol result = symtbl.getSymbol(ctx._results[0]);
        if (result._type == itRASTER){
            if (result._var.canConvert<Ilwis::IRasterCoverage>()){
                Ilwis::IRasterCoverage* obj = new Ilwis::IRasterCoverage(result._var.value<Ilwis::IRasterCoverage>());
                if (rename)
                    (*obj)->name(QString("%1_%2").arg(operation.c_str()).arg((*obj)->id()));
                return new RasterCoverage(obj);
            }
        }else if (result._type == itFEATURE){
            if (result._var.canConvert<Ilwis::IFeatureCoverage>()){
                Ilwis::IFeatureCoverage* obj = new Ilwis::IFeatureCoverage(result._var.value<Ilwis::IFeatureCoverage>());
                if (rename)
                    (*obj)->name(QString("%1_%2").arg(operation.c_str()).arg((*obj)->id()));
                return new FeatureCoverage(obj);
            }
        }else if (result._type == itCOORDSYSTEM){
            if (result._var.canConvert<Ilwis::ICoordinateSystem>()){
                Ilwis::ICoordinateSystem* obj = new Ilwis::ICoordinateSystem(result._var.value<Ilwis::ICoordinateSystem>());
                if (rename)
                    (*obj)->name(QString("%1_%2").arg(operation.c_str()).arg((*obj)->id()));
                return new CoordinateSystem(obj);
            }
        }else if (result._type == itGEOREF){
            if (result._var.canConvert<Ilwis::IGeoReference>()){
                Ilwis::IGeoReference* obj = new Ilwis::IGeoReference(result._var.value<Ilwis::IGeoReference>());
                if (rename)
                    (*obj)->name(QString("%1_%2").arg(operation.c_str()).arg((*obj)->id()));
                return new GeoReference(obj);
            }
        } else if (result._type == itBOOL){

        }
        throw Ilwis::ErrorObject(QString("couldn't handle return type of do(%1)").arg(command));
    }else{
        throw Ilwis::ErrorObject(QString("couldn't do(%1)").arg(command));
    }
}

void Engine::setWorkingCatalog(const std::string& location) {
    QString loc (QString::fromStdString(location));
    loc.replace('\\','/');
    // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
    if (loc.indexOf("://") < 0) {
        int pos = loc.indexOf('/');
        if (pos > 0) { // full path starting with drive-letter (MS-DOS-style)
            loc = "file:///" + loc;
            if (loc.endsWith('/')) // workaround an IlwisObjects problem that scans the folder twice if it ends with a slash
                loc = loc.left(loc.length() - 1);
        } else if (pos == 0) { // full path starting with path-separator (UNIX-style)
            loc = "file://" + loc;
            if (loc.endsWith('/'))
                loc = loc.left(loc.length() - 1);
        }
    }

    Ilwis::ICatalog cat;
    cat.prepare(loc);
    if(cat.isValid()){
        Ilwis::context()->setWorkingCatalog(cat);
    }else
        throw Ilwis::ErrorObject(QString("invalid container location: '%1'").arg(location.c_str()));
}

std::string Engine::getLocation(){
    Ilwis::ICatalog cat = Ilwis::context()->workingCatalog();
    QUrl location = cat->filesystemLocation();
    return location.toString().toStdString();
}

PyObject* Engine::operations(const std::string& filter){
    Ilwis::CatalogView opCat(QUrl(QString("ilwis://operations")));
    opCat.filter("type='SingleOperation' or type='Workflow'");
    opCat.prepare();
    std::vector<Ilwis::Resource> ops = opCat.items();
    PyObject* list = newPyTuple(ops.size());
    int i = 0;
    for(auto it = ops.begin(); it != ops.end(); it++){
        if (!setTupleItem(list, i++, PyUnicodeFromString(it->name().toStdString().data()))){
            throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of attributes").arg( it->name()));
        }
    }
    return list;
}

std::string Engine::operationMetaData(const std::string &name, const std::string &element){
    Ilwis::CatalogView opCat(QUrl(QString("ilwis://operations")));
    opCat.filter("type='SingleOperation' or type='Workflow'");
    opCat.prepare();
    std::vector<Ilwis::Resource> ops = opCat.items();
    QString ret;
    for(auto it = ops.begin();it != ops.end(); it++){
        if (QString::fromStdString(name).compare(it->name(),Qt::CaseInsensitive) == 0){
            if(!ret.isEmpty())
                ret.append("\n");
            if (element.compare("description") == 0)
                ret.append(it->description());
            else
                ret.append((*it)[element.c_str()].toString());
        }
    }
    return ret.toStdString();
}

PyObject* Engine::_catalogItems(){
    Ilwis::ICatalog cat = Ilwis::context()->workingCatalog();
    std::vector<Ilwis::Resource> resVec = cat->items();
    PyObject* tup = newPyTuple(resVec.size());
    int i = 0;
    for(auto it = resVec.begin();it != resVec.end(); it++){
        if (!setTupleItem(tup, i++, PyUnicodeFromString(it->name().toStdString().data()))){
            throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of files").arg( it->name()));
        }
    }
    return tup;
}
