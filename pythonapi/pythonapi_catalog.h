#ifndef PYTHONAPI_CATALOG_H
#define PYTHONAPI_CATALOG_H

#include "pythonapi_ilwisobject.h"

namespace Ilwis {
    class CatalogView;
}

typedef struct _object PyObject;

namespace pythonapi {

    class Catalog : public Object{
        public:
            Catalog(const std::string& url, const std::string& filter = "");
            ~Catalog();

            bool __bool__() const;
            std::string __str__();
            IlwisTypes ilwisType();

            PyObject* items();
            IlwisObject* _getitem(const std::string &name);
    protected:
            std::unique_ptr<Ilwis::CatalogView> _data;
    };

} // namespace pythonapi

#endif // PYTHONAPI_CATALOG_H
