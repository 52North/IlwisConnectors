#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericdomain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "numericitem.h"
#include "identifieritem.h"
#include "thematicitem.h"

#include "pythonapi_pyobject.h"
#include "pythonapi_range.h"
#include "pythonapi_domain.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_error.h"


namespace pythonapi{

Domain::Domain(){
}

Domain::Domain(Ilwis::IDomain *domain) : IlwisObject(new Ilwis::IIlwisObject(*domain)){
    delete domain;
}

bool Domain::isStrict() const{
    return this->ptr()->get<Ilwis::Domain>()->isStrict();
}

void Domain::setStrict(bool yesno){
    this->ptr()->get<Ilwis::Domain>()->setStrict(yesno);
}

PyObject* Domain::impliedValue(PyObject *value) const{
    return value;
}

Domain Domain::parent() const{
    Ilwis::IDomain *domain = new Ilwis::IDomain();
    unsigned long long id = this->ptr()->get<Ilwis::Domain>()->parent()->id();
    (*domain).prepare(id);
    return Domain( domain);
}

void Domain::setParent(const Domain &dom){
    this->ptr()->get<Ilwis::Domain>()->setParent(dom.ptr()->get<Ilwis::Domain>());
}

Domain::Containement Domain::contains(PyObject* value) const{
    std::unique_ptr<QVariant> var(PyObject2QVariant(value));
    return (Domain::Containement)this->ptr()->get<Ilwis::Domain>()->contains(*var);
}

bool Domain::isCompatibleWith(const Domain &dom) const{
    return this->ptr()->get<Ilwis::Domain>()->isCompatibleWith(dom.ptr()->get<Ilwis::Domain>());
}

void Domain::setRange(const Range &rng){
    return this->ptr()->get<Ilwis::Domain>()->range(rng._range->clone());
}

IlwisTypes Domain::valueType() const
{
    return this->ptr()->get<Ilwis::Domain>()->valueType();
}

Domain *Domain::toDomain(Object *obj){
    Domain* ptr = dynamic_cast<Domain*>(obj);
    if(!ptr)
        throw InvalidObject("cast to Domain not possible");
    return ptr;
}

//--------------------------------------------------------------
NumericDomain::NumericDomain(const std::string &resource){
    Ilwis::INumericDomain numdom(QString::fromStdString(resource), itNUMERICDOMAIN);
    if (numdom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(numdom));
}

NumericDomain *NumericDomain::toNumericDomain(Object *obj){
    NumericDomain* ptr = dynamic_cast<NumericDomain*>(obj);
    if(!ptr)
        throw InvalidObject("cast to NumericDomain not possible");
    return ptr;
}

NumericDomain::NumericDomain(Ilwis::INumericDomain *domain): Domain(new Ilwis::IDomain(*domain)){
    delete domain;
}



ItemDomain::ItemDomain()
{

}

PyObject *ItemDomain::item(int index, bool labelOnly)
{
    Ilwis::SPDomainItem domitem = this->ptr()->get<Ilwis::NumericItemDomain>()->item(index);
    IlwisTypes vt = valueType();
    switch (vt) {
    case itNUMERICITEM:{
        Ilwis::NumericItem *numitem = domitem->toType<Ilwis::NumericItem>();
        PyObject* list;
        if ( labelOnly)
            list = newPyTuple(1);
        else
            list = newPyTuple(4);
        setTupleItem(list, 0, PyUnicodeFromString(numitem->name().toLatin1()));
        if (!labelOnly){
            setTupleItem(list, 1, PyFloatFromDouble(numitem->range().min()));
            setTupleItem(list, 2, PyFloatFromDouble(numitem->range().max()));
            setTupleItem(list, 3, PyFloatFromDouble(numitem->range().resolution()));
        }
        return list;
    }
        break;
    case itTHEMATICITEM:{
        Ilwis::ThematicItem *titem = domitem->toType<Ilwis::ThematicItem>();
        PyObject* list;
        if ( labelOnly)
            list = newPyTuple(1);
        else
            list = newPyTuple(3);
        setTupleItem(list, 0, PyUnicodeFromString(titem->name().toLatin1()));
        if(!labelOnly){
            setTupleItem(list, 0, PyUnicodeFromString(titem->code().toLatin1()));
            setTupleItem(list, 0, PyUnicodeFromString(titem->description().toLatin1()));
        }
        return list;
    }
        break;
    case itNAMEDITEM:
    case itINDEXEDITEM:
    {
        PyObject* list = newPyTuple(1);
        setTupleItem(list, 0, PyUnicodeFromString(domitem->name().toLatin1()));
        return list;
    }
        break;
    default:
        break;
    }
    return 0;
}
}
