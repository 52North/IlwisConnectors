#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domainitem.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/util/range.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"

#include "pythonapi_datadefinition.h"
#include "pythonapi_error.h"
#include "pythonapi_extension.h"
#include "pythonapi_domain.h"
#include "pythonapi_range.h"

using namespace pythonapi;

DataDefinition::DataDefinition() : _ilwisDatadef(new Ilwis::DataDefinition()){
}

DataDefinition::DataDefinition(Ilwis::DataDefinition def) : _ilwisDatadef(new Ilwis::DataDefinition(def)){
}

DataDefinition::DataDefinition(Domain* dm, const Range &rng){
    if(dm->__bool__() && rng.__bool__()){
        this->_ilwisDatadef.reset(new Ilwis::DataDefinition(dm->ptr()->as<Ilwis::Domain>(), rng._range.get()));
    }
    delete dm;
}

DataDefinition::DataDefinition(const DataDefinition &def){
    if(def.__bool__())
        this->_ilwisDatadef.reset(new Ilwis::DataDefinition(def.ptr()));
}


DataDefinition::~DataDefinition(){
    this->ptr().~DataDefinition();
}

bool DataDefinition::__bool__() const{
    return (bool)this->_ilwisDatadef && this->_ilwisDatadef->isValid();
}

void DataDefinition::range(const Range &rng){
    this->ptr().range(rng._range.get());
}

void DataDefinition::domain(Domain* dm){
    this->ptr().domain(dm->ptr()->as<Ilwis::Domain>());
    delete dm;
}

bool DataDefinition::isCompatibleWith(const DataDefinition& def){
    return this->ptr().isCompatibleWith(def.ptr());
}

DataDefinition* DataDefinition::merge(const DataDefinition& def1, const DataDefinition& def2){
    DataDefinition* datdef = new DataDefinition();

    Ilwis::DataDefinition ilwdef = this->ptr().merge(def1.ptr(), def2.ptr());

    datdef->_ilwisDatadef.reset(&ilwdef);
    return datdef;
}

DataDefinition* DataDefinition::operator=(const DataDefinition& def){
    this->ptr() = def.ptr();
    return this;
}

Ilwis::DataDefinition& DataDefinition::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("use of invalid DataDefinition(ptr)");
    return (*this->_ilwisDatadef);
}
