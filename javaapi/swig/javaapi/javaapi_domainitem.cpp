#include <cstddef>
#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "javaapi_qtGNUTypedefs.h"

#include "../../IlwisCore/core/ilwisobjects/domain/itemrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domainitem.h"
#include "../../IlwisCore/core/ilwisobjects/domain/identifieritem.h"
#include "../../IlwisCore/core/ilwisobjects/domain/thematicitem.h"
#include "../../IlwisCore/core/ilwisobjects/domain/interval.h"

#include "javaapi_range.h"
#include "javaapi_error.h"
#include "javaapi_domainitem.h"


using namespace javaapi;

//==============================================Domain Item==============================================================

DomainItem::DomainItem(){

}

DomainItem::DomainItem(Ilwis::DomainItem* ilwItem) : _ilwisItem(ilwItem){

}

//just a dummy constructor for template methods in rangeiterator
DomainItem::DomainItem(double d){

}

bool DomainItem::__bool__() const{
    if(this->_ilwisItem != NULL && this->_ilwisItem->isValid()){
        return this->ptr()->isValid();
    } else{
        return false;
    }
}

std::string DomainItem::__str__(){
    QString qName = this->ptr()->name();
    return qName.toStdString();
}

IlwisTypes DomainItem::ilwisType(){
    return this->ptr()->valueType();
}

DomainItem* DomainItem::clone(){
    return new DomainItem(this->ptr()->clone());
}

void DomainItem::raw(quint32 rw){
    this->ptr()->raw(rw);
}

quint32 DomainItem::raw(){
    return this->ptr()->raw();
}

std::shared_ptr<Ilwis::DomainItem> DomainItem::ptr() const{
    //if (!this->__bool__()) //TODO error handling
       // throw InvalidObject("invalid Item");
    return this->_ilwisItem;
}


//==============================================IndexedIdentifier==============================================================

IndexedIdentifier::IndexedIdentifier(){

}

IndexedIdentifier::IndexedIdentifier(const std::string& label, quint32 ind, qint32 cnt){
    this->_ilwisItem.reset(new Ilwis::IndexedIdentifier(QString::fromStdString(label), ind, cnt));
}

IndexedIdentifier::IndexedIdentifier(Ilwis::DomainItem *ilwItem){
    this->_ilwisItem.reset(ilwItem);
}

std::string IndexedIdentifier::prefix(){
    QString qStr = static_cast<Ilwis::IndexedIdentifier*>(this->ptr().get())->prefix();
    return qStr.toStdString();
}

void IndexedIdentifier::setPrefix(const std::string &name){
    static_cast<Ilwis::IndexedIdentifier*>(this->ptr().get())->setPrefix(QString::fromStdString(name));
}

bool IndexedIdentifier::operator==(IndexedIdentifier& item) const{
    Ilwis::IndexedIdentifier* item1 = static_cast<Ilwis::IndexedIdentifier*>(this->ptr().get());
    Ilwis::IndexedIdentifier* item2 = static_cast<Ilwis::IndexedIdentifier*>(item.ptr().get());
    return item1 == item2;
}

//==============================================NamedIdentifier==============================================================

NamedIdentifier::NamedIdentifier(){

}

NamedIdentifier::NamedIdentifier(const std::string& name, quint32 rawvalue){
    this->_ilwisItem.reset(new Ilwis::NamedIdentifier(QString::fromStdString(name), rawvalue));
}

NamedIdentifier::NamedIdentifier(Ilwis::DomainItem *ilwItem){
    this->_ilwisItem.reset(ilwItem);
}

void NamedIdentifier::setName(const std::string &name){
    static_cast<Ilwis::NamedIdentifier*>(this->ptr().get())->setName(QString::fromStdString(name));
}

bool NamedIdentifier::operator==(NamedIdentifier& item) const{
    Ilwis::NamedIdentifier* item1 = static_cast<Ilwis::NamedIdentifier*>(this->ptr().get());
    Ilwis::NamedIdentifier* item2 = static_cast<Ilwis::NamedIdentifier*>(item.ptr().get());
    return item1 == item2;
}

//==============================================Thematic Item==============================================================

ThematicItem::ThematicItem(){

}

ThematicItem::ThematicItem(Ilwis::DomainItem *ilwItem){
    this->_ilwisItem.reset(ilwItem);
}
/*
ThematicItem::ThematicItem(PyObject* tup, quint32 rawValue){
    int sz = CppTupleElementCount(tup);
    QStringList qList;
    QString qStr;
    for(int i=0; i < sz; i++){
        qList.append(qStr.fromStdString(CppTupleElement2String(tup, i)));
    }
    this->_ilwisItem.reset(new Ilwis::ThematicItem(qList, rawValue));
}*/

std::string ThematicItem::description(){
    QString qStr = static_cast<Ilwis::ThematicItem*>(this->ptr().get())->description();
    return qStr.toStdString();
}

void ThematicItem::description(const std::string& descr){
    static_cast<Ilwis::ThematicItem*>(this->ptr().get())->description(QString::fromStdString(descr));
}

std::string ThematicItem::code(){
    QString qStr = static_cast<Ilwis::ThematicItem*>(this->ptr().get())->code();
    return qStr.toStdString();
}

void ThematicItem::code(const std::string& code){
    static_cast<Ilwis::ThematicItem*>(this->ptr().get())->code(QString::fromStdString(code));
}


//==============================================Interval==============================================================

Interval::Interval(){

}

Interval::Interval(const std::string &label, NumericRange &nr){
    Ilwis::NumericRange* ilwNumRan = static_cast<Ilwis::NumericRange*>(nr._range.get());
    _ilwisItem.reset(new Ilwis::Interval(QString::fromStdString(label), *ilwNumRan));
}

bool Interval::isOrdered() const{
    return static_cast<Ilwis::Interval*>(this->ptr().get())->isOrdered();
}

NumericRange Interval::range(){
    Ilwis::NumericRange* nr = new Ilwis::NumericRange(static_cast<Ilwis::Interval*>(this->ptr().get())->range());
    return NumericRange(nr);
}

void Interval::range(const NumericRange &nr){
    Ilwis::NumericRange* ilwNumRan = static_cast<Ilwis::NumericRange*>(nr._range.get());
   static_cast<Ilwis::Interval*>(this->ptr().get())->range(*ilwNumRan);
}

