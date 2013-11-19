#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"

#include "coverage.h"

using namespace pythonapi;

Coverage::Coverage():IlwisObject(){
}

bool Coverage::addAttribute(const char *name, const char *domain){
    return (*this->ptr()).get<Ilwis::Coverage>()->attributeTable()->addColumn(QString(name),QString(domain));
}

quint32 Coverage::attributeCount(){
    return (*this->ptr()).get<Ilwis::Coverage>()->attributeTable()->columnCount();
}

std::vector<std::string> Coverage::attributes(){
    std::vector<std::string> ret;
    Ilwis::ITable tbl = (*this->ptr()).get<Ilwis::Coverage>()->attributeTable();
    for(int i = 0; i < tbl->columnCount(); i++){
        ret.push_back(tbl->columndefinition(i).name().toStdString());
    }
    return ret;
}

