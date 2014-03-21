#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericdomain.h"

#include "pythonapi_range.h"
#include "pythonapi_domain.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_error.h"


using namespace pythonapi;

Domain::Domain()
{
}

Domain::Domain(Ilwis::IDomain *domain) : IlwisObject(new Ilwis::IIlwisObject(*domain))
{
}

bool Domain::isStrict() const
{
    return this->ptr()->get<Ilwis::Domain>()->isStrict();
}

void Domain::setStrict(bool yesno)
{
    this->ptr()->get<Ilwis::Domain>()->setStrict(yesno);
}

IlwisTypes Domain::valueType() const
{
    return itUNKNOWN;
}

PyObject *Domain::impliedValue(PyObject *value) const
{
    return value;
}

Domain Domain::parent() const
{
    Ilwis::IDomain *domain = new Ilwis::IDomain();
    unsigned long long id = this->ptr()->get<Ilwis::Domain>()->parent()->id();
    (*domain).prepare(id);
    return Domain( domain);
}

void Domain::setParent(const Domain &dom)
{

    this->ptr()->get<Ilwis::Domain>()->setParent(dom.ptr()->get<Ilwis::Domain>());
}

Ilwis::Domain::Containement Domain::contains(PyObject *value) const
{
    std::unique_ptr<QVariant> var(PyObject2QVariant(value));
    return this->ptr()->get<Ilwis::Domain>()->contains(*var);
}

bool Domain::isCompatibleWith(const Domain &dom) const
{
    return this->ptr()->get<Ilwis::Domain>()->isCompatibleWith(dom.ptr()->get<Ilwis::Domain>());
}

void Domain::setRange(const Range &rng)
{
    return this->ptr()->get<Ilwis::Domain>()->range(rng._range->clone());
}



//--------------------------------------------------------------
NumericDomain::NumericDomain(const std::string &resource)
{
    Ilwis::INumericDomain numdom(QString::fromStdString(resource), itNUMERICDOMAIN);
    if (numdom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(numdom));
}

NumericDomain *NumericDomain::toNumericDomain(Object *obj)
{
    NumericDomain* ptr = dynamic_cast<NumericDomain*>(obj);
    if(!ptr)
        throw InvalidObject("cast to NumericDomain not possible");
    return ptr;
}
