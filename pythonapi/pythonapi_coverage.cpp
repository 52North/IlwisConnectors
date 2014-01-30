#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"

#include "pythonapi_coverage.h"
#include "pythonapi_pycontainer.h"


using namespace pythonapi;

Coverage::Coverage():IlwisObject(){}

Coverage::Coverage(Ilwis::ICoverage *coverage):IlwisObject(new Ilwis::IIlwisObject(*coverage)){
}

bool Coverage::addAttribute(const std::string& name, const std::string& domain){
    return this->ptr()->get<Ilwis::Coverage>()->attributeTable()->addColumn(QString::fromStdString(name),QString::fromStdString(domain));
}

quint32 Coverage::attributeCount(){
    return this->ptr()->get<Ilwis::Coverage>()->attributeTable()->columnCount();
}

PyObject* Coverage::attributes(){
    Ilwis::ITable tbl = (*this->ptr()).get<Ilwis::Coverage>()->attributeTable();
    int colCount = tbl->columnCount();
    if (tbl->columndefinition(FEATUREIDCOLUMN).isValid())
        colCount--;
    int offset = 0;
    PyObject* list = newPyTuple(colCount);//skip 'feature_id'
    for(int i = 0; i < tbl->columnCount(); i++){
        QString name = tbl->columndefinition(i).name();
        if (name != QString(FEATUREIDCOLUMN)){
            if (!setTupleItem(list, i-offset, name.toStdString().data()))
                throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of attributes").arg(tbl->columndefinition(i).name()));
        }else{
            offset++;
        }
    }
    return list;
}

Table Coverage::attributeTable(AttributeType attType){
    return Table(new Ilwis::ITable(this->ptr()->get<Ilwis::Coverage>()->attributeTable((Ilwis::Coverage::AttributeType)attType)));
}

CoordinateSystem Coverage::coordinateSystem(){
    return CoordinateSystem(new Ilwis::ICoordinateSystem(this->ptr()->get<Ilwis::Coverage>()->coordinateSystem()));
}

void Coverage::setCoordinateSystem(const CoordinateSystem &cs){
    this->ptr()->get<Ilwis::Coverage>()->setCoordinateSystem(cs.ptr()->get<Ilwis::CoordinateSystem>());
}

