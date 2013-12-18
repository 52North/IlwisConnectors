#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"

#include "pythonapi_coverage.h"
#include "pythonapi_container.h"


using namespace pythonapi;

Coverage::Coverage(Ilwis::ICoverage *coverage):IlwisObject(new Ilwis::IIlwisObject(*coverage)){
}

Coverage::Coverage():IlwisObject(){
}

bool Coverage::addAttribute(const std::string& name, const std::string& domain){
    return (*this->ptr()).get<Ilwis::Coverage>()->attributeTable()->addColumn(QString::fromStdString(name),QString::fromStdString(domain));
}

quint32 Coverage::attributeCount(){
    return (*this->ptr()).get<Ilwis::Coverage>()->attributeTable()->columnCount();
}

PyObject* Coverage::attributes(){
    Ilwis::ITable tbl = (*this->ptr()).get<Ilwis::Coverage>()->attributeTable();
    PyObject* list = newPyTuple(tbl->columnCount()-1);//skip 'feature_id'
    for(int i = 1; i < tbl->columnCount(); i++){
        if (!setTupleItem(list, i-1,tbl->columndefinition(i).name().toStdString().data()))
            throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of attributes").arg(tbl->columndefinition(i).name()));
    }
    return list;
}

CoordinateSystem Coverage::coordinateSystem(){
    return CoordinateSystem(new Ilwis::ICoordinateSystem(this->ptr()->get<Ilwis::Coverage>()->coordinateSystem()));
}

void Coverage::setCoordinateSystem(const CoordinateSystem &cs){
    this->ptr()->get<Ilwis::Coverage>()->setCoordinateSystem(cs.ptr()->get<Ilwis::CoordinateSystem>());
}

