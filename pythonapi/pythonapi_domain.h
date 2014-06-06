#ifndef PYTHONAPI_DOMAIN_H
#define PYTHONAPI_DOMAIN_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_pyobject.h"

namespace Ilwis {
    class Domain;
    typedef IlwisData<Domain> IDomain;

    class NumericDomain;
    typedef IlwisData<NumericDomain> INumericDomain;

}

namespace pythonapi {

    class Range;
    class Color;
    class ItemRange;

    class Domain : public IlwisObject{
    friend class DataDefinition;
    public:
        enum Containement{cSELF=1, cPARENT=2, cDECLARED=3, cNONE=0};

        Domain();
        virtual ~Domain();

        bool isStrict() const;
        void setStrict(bool yesno);

        virtual PyObject* impliedValue(PyObject *value) const;
        Domain parent() const;
        void setParent(const Domain& dom);

        std::string contains(PyObject* value) const;
        bool isCompatibleWith(const Domain& dom) const;
        void setRange(const Range& rng);
        IlwisTypes valueType() const;
        static Domain* toDomain(Object* obj);

    protected:
        Domain(Ilwis::IDomain *domain);
    };

    class NumericDomain : public Domain{
        friend class Catalog;
    public:
        NumericDomain();
        NumericDomain(const std::string& resource);
        static NumericDomain* toNumericDomain(Object *obj);


    private:
        NumericDomain(Ilwis::INumericDomain* domain);
    };



    class ItemDomain : public Domain {
    public:
        ItemDomain();
        ItemDomain(const Range& rng);
        quint32 count();
        void setTheme(const std::string &theme);
        std::string theme();
        void removeItem(const std::string& nme);
        void addItem(PyObject* value);
        PyObject *item(int index, bool labelOnly);

    private:
        ItemDomain(Ilwis::IDomain *domain);


    };

    class ColorDomain : public Domain {
    public:
        ColorDomain();
        ColorDomain(const std::string& resource);
        IlwisTypes ilwisType() const;
        std::string containsColor(const Color &value) const;

    };

    class TextDomain : public Domain {
    public:
        TextDomain();
        TextDomain(const std::string& resource);
    };

    class TimeDomain : public Domain{
        public:
            TimeDomain();
            TimeDomain(const std::string& resource);
            TimeDomain(const Range& rng);
        };
}


#endif // PYTHONAPI_DOMAIN_H
