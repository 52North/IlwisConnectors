#ifndef JAVAAPI_CATALOG_H
#define JAVAAPI_CATALOG_H

#include "javaapi_ilwisobject.h"

namespace Ilwis {
    class CatalogView;
}

namespace javaapi {

    class Catalog : public IObject{
        public:
            Catalog(const std::string& url, const std::string& filter = "");
            ~Catalog();

            bool __bool__() const;
            std::string __str__();
            IlwisTypes ilwisType();

            std::vector<std::string> items();
            IObject* _getitem(const std::string &name);

            Catalog* toCatalog(IObject* obj);
    protected:
            Catalog(Ilwis::CatalogView* cat);
            std::unique_ptr<Ilwis::CatalogView> _data;
    };

} // namespace javaapi

#endif // JAVAAPI_CATALOG_H
