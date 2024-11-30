#ifndef JAVAAPI_DOMAINITEM_H
#define JAVAAPI_DOMAINITEM_H

#include "javaapi_object.h"
#include "javaapi_qtGNUTypedefs.h"
#include <memory>

namespace Ilwis{
class DomainItem;
}

namespace javaapi{
    const double rUNDEF = -1e308;
    const long   iUNDEF  = 2147483645;
    const long long i64UNDEF = std::numeric_limits < qint64 >::min();

    class NumericRange;

    class DomainItem : public IObject{

        template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange>
        friend class RangeIterator;

    public:
        DomainItem();
        DomainItem(Ilwis::DomainItem* ilwItem);

        bool __bool__() const;
        std::string __str__();
        IlwisTypes ilwisType();

        DomainItem* clone();
        void raw(quint32 rw);
        quint32 raw();

    protected:
        std::shared_ptr<Ilwis::DomainItem> ptr() const;
        std::shared_ptr<Ilwis::DomainItem> _ilwisItem;

        //just a dummy constructor for template methods in rangeiterator
        DomainItem(double d);
    };

//==============================================IndexedIdentifier============================================================

        class IndexedIdentifier : public DomainItem{
        public:
                IndexedIdentifier();
                IndexedIdentifier(const std::string& label, quint32 ind=0, qint32 cnt =iUNDEF);
                IndexedIdentifier(Ilwis::DomainItem* ilwItem);

                std::string prefix();
                void setPrefix(const std::string& name);
                bool operator==(IndexedIdentifier& item) const;
        };



//==============================================NamedIdentifier============================================================

    class NamedIdentifier : public DomainItem{
    public:
            NamedIdentifier();
            NamedIdentifier(const std::string& name, quint32 rawvalue=iUNDEF);
            NamedIdentifier(Ilwis::DomainItem* ilwItem);

            void setName(const std::string& name);
            bool operator==(NamedIdentifier& item) const;
    };

//==============================================Thematic Item============================================================

    class ThematicItem : public NamedIdentifier{
    public:
        ThematicItem();
        ThematicItem(Ilwis::DomainItem* ilwItem);
        //ThematicItem(PyObject* tup, quint32 rawValue=iUNDEF);

        std::string description();
        void description(const std::string& descr);
        std::string code();
        void code(const std::string& code);
    };

//==============================================Interval==============================================================

   class Interval : public DomainItem{
   public:
       Interval();
       Interval(const std::string& label, NumericRange& nr);
       bool isOrdered() const;
       NumericRange range();
       void range(const NumericRange& nr);
   };


}

#endif // JAVAAPI_DOMAINITEM_H
