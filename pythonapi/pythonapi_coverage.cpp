#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"

#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"

#include "pythonapi_domain.h"
#include "pythonapi_coverage.h"
#include "pythonapi_pyobject.h"
#include "pythonapi_util.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_columndefinition.h"

using namespace pythonapi;

Coverage::Coverage():IlwisObject(){}

Coverage::Coverage(Ilwis::ICoverage *coverage):IlwisObject(new Ilwis::IIlwisObject(*coverage)){
    delete coverage;
}

bool Coverage::addAttribute(const std::string& name, const std::string& domain){
    return this->ptr()->as<Ilwis::Coverage>()->attributeTable()->addColumn(QString::fromStdString(name),QString::fromStdString(domain));
}

bool Coverage::addAttribute(const std::string& name, const Domain& domain){
    Ilwis::IDomain ilwDom = domain.ptr()->as<Ilwis::Domain>();
    return this->ptr()->as<Ilwis::Coverage>()->attributeTable()->addColumn(QString::fromStdString(name), ilwDom);
}

bool Coverage::addAttribute( const ColumnDefinition& def){
    Ilwis::ColumnDefinition* ilwColdef = def.ptr().get();
    return this->ptr()->as<Ilwis::Coverage>()->attributeTable()->addColumn(*ilwColdef);
}

quint32 Coverage::attributeCount(){
    return this->ptr()->as<Ilwis::Coverage>()->attributeTable()->columnCount();
}

PyObject* Coverage::attributes(){
    Ilwis::ITable tbl = (*this->ptr()).as<Ilwis::Coverage>()->attributeTable();
    int colCount = tbl->columnCount();
    if (tbl->columndefinition(FEATUREIDCOLUMN).isValid())
        colCount--;//skip 'feature_id'
    int offset = 0;
    PyObject* list = newPyTuple(colCount);
    for(int i = 0; i < tbl->columnCount(); i++){
        QString name = tbl->columndefinition(i).name();
        if (name != QString(FEATUREIDCOLUMN)){
            if (!setTupleItem(list, i-offset, PyUnicodeFromString(name.toStdString().data())))
                throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of attributes").arg(tbl->columndefinition(i).name()));
        }else{
            offset++;
        }
    }
    return list;
}

Table Coverage::attributeTable(AttributeType attType){
    return Table(new Ilwis::ITable(this->ptr()->as<Ilwis::Coverage>()->attributeTable((Ilwis::Coverage::AttributeType)attType)));
}

void Coverage::setTable(Table& tbl, AttributeType attType){
    Ilwis::ITable ilwTab = tbl.ptr()->as<Ilwis::Table>();
    this->ptr()->as<Ilwis::Coverage>()->attributeTable(ilwTab, (Ilwis::Coverage::AttributeType)attType);
}

bool Coverage::hasAttributes(AttributeType attType) const{
    return this->ptr()->as<Ilwis::Coverage>()->hasAttributes((Ilwis::Coverage::AttributeType)attType);
}

CoordinateSystem Coverage::coordinateSystem(){
    return CoordinateSystem(new Ilwis::ICoordinateSystem(this->ptr()->as<Ilwis::Coverage>()->coordinateSystem()));
}

void Coverage::setCoordinateSystem(const CoordinateSystem &cs){
    this->ptr()->as<Ilwis::Coverage>()->coordinateSystem(cs.ptr()->as<Ilwis::CoordinateSystem>());
}

Envelope Coverage::envelope(){
    return Envelope(this->ptr()->as<Ilwis::Coverage>()->envelope());
}

void Coverage::setEnvelope(const Envelope &env){
    this->ptr()->as<Ilwis::Coverage>()->envelope(env.data());
}

void Coverage::indexDomain(const Domain &dom){
    this->ptr()->as<Ilwis::Coverage>()->indexDomain(Ilwis::IDomain(dom.ptr()->as<Ilwis::Domain>()));
}

PyObject* Coverage::indexValues(){
    std::vector<QVariant> ilwVal = this->ptr()->as<Ilwis::Coverage>()->indexValues();
    PyObject* tup = newPyTuple(ilwVal.size());
    for(int i = 0; i < ilwVal.size(); i++){
            PyObject* actVal = QVariant2PyObject(ilwVal[i]);
            setTupleItem(tup, i, actVal);
    }
    return tup;
}

PyObject* Coverage::value(const std::string &colName, quint32 itemid, qint32 layerIndex){
    QVariant qVar = this->ptr()->as<Ilwis::Coverage>()->value(QString::fromStdString(colName), itemid, layerIndex);
    return QVariant2PyObject(qVar);
}

