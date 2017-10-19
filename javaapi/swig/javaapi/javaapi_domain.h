#ifndef JAVAAPI_DOMAIN_H
#define JAVAAPI_DOMAIN_H

#include "javaapi_ilwisobject.h"

namespace Ilwis {
    class Domain;
    typedef IlwisData<Domain> IDomain;

    class NumericDomain;
    typedef IlwisData<NumericDomain> INumericDomain;

}

namespace javaapi {

    class Range;
    class Color;
    class ItemRange;

    class Domain : public IlwisObject{
    friend class DataDefinition;
    friend class ColumDefinition;
    friend class FeatureCoverage;
    friend class RasterCoverage;
    public:
        enum Containement{cSELF=1, cPARENT=2, cDECLARED=3, cNONE=0};

        Domain();
        virtual ~Domain();

        bool isStrict() const;
        void setStrict(bool yesno);

        //virtual PyObject* impliedValue(PyObject *value) const;
        Domain parent() const;
        void setParent(const Domain& dom);

        Range& getRange();
        std::string contains(double value) const;
        std::string contains(const std::string& value) const;
        bool isCompatibleWith(const Domain& dom) const;
        void setRange(const Range& rng);
        IlwisTypes valueType() const;
        static Domain* toDomain(IObject* obj);

    protected:
        Domain(Ilwis::IDomain *domain);
    };

    class NumericDomain : public Domain{
        friend class Catalog;
        friend class DataDefinition;
        friend class ColumDefinition;
    public:
        NumericDomain();
        NumericDomain(const std::string& resource);
        static NumericDomain* toNumericDomain(IObject *obj);


    protected:
        NumericDomain(Ilwis::INumericDomain* domain);
    };

    class ItemDomain : public Domain {
        friend class DataDefinition;
        friend class ColumDefinition;
    public:
        ItemDomain();
        ItemDomain(const Range& rng);
        quint32 count();
        void setTheme(const std::string &theme);
        std::string theme();
        void removeItem(const std::string& nme);
        //void addItem(PyObject* value);
        //QVariant *item(int index, bool labelOnly);
        void range(const Range &rng);

    protected:
        ItemDomain(Ilwis::IDomain *domain);


    };

    class ColorDomain : public Domain {
        friend class DataDefinition;
        friend class ColumDefinition;
    public:
        ColorDomain();
        ColorDomain(const std::string& resource);
        IlwisTypes ilwisType() const;
        std::string containsColor(const Color &value) const;

    };

    class TextDomain : public Domain {
        friend class DataDefinition;
        friend class ColumDefinition;
    public:
        TextDomain();
        TextDomain(const std::string& resource);
    };

    class TimeDomain : public Domain{
        friend class DataDefinition;
        friend class ColumDefinition;
        public:
            TimeDomain();
            TimeDomain(const std::string& resource);
            TimeDomain(const Range& rng);
        };
}


#endif // JAVAAPI_DOMAIN_H
