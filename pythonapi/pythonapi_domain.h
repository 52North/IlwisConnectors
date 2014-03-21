#ifndef PYTHONAPI_DOMAIN_H
#define PYTHONAPI_DOMAIN_H

#include "pythonapi_ilwisobject.h"

namespace Ilwis {
    class Domain;
    typedef IlwisData<Domain> IDomain;

    class NumericDomain;
    typedef IlwisData<NumericDomain> INumericDomain;
}

namespace pythonapi {

class Domain : public IlwisObject
{
public:
    //enum Containement{cSELF=1, cPARENT=2, cDECLARED=3, cNONE=0};

    Domain();

    bool isStrict() const;
    void setStrict(bool yesno);

    virtual IlwisTypes valueType() const ;
    virtual PyObject* impliedValue(PyObject *value) const;
    Domain parent() const;
    void setParent(const Domain& dom);

    Ilwis::Domain::Containement contains(PyObject *value) const;
    bool isCompatibleWith(const Domain& dom) const;
    void setRange(const Range& rng);

private:
    Domain(Ilwis::IDomain *domain);
};

class NumericDomain : public Domain
{
public:
    NumericDomain(const std::string& resource);
    static NumericDomain *toNumericDomain(Object *obj);


private:
    NumericDomain(Ilwis::INumericDomain *dom);
};
}

#endif // PYTHONAPI_DOMAIN_H
