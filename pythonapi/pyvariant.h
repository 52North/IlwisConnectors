#ifndef PYTHONAPI_PYVARIANT_H
#define PYTHONAPI_PYVARIANT_H

#include "object.h"
#include <memory>

class QVariant;

namespace pythonapi{

    class PyVariant: public Object{
    public:
        PyVariant();
        PyVariant(QVariant* data);
        ~PyVariant();
        void __del__();
        const char* __str__();
        int __int__();
        bool __bool__() const;
        IlwisTypes ilwisType();
        static PyVariant* toPyVariant(Object* obj);
    protected:
        std::unique_ptr<QVariant> _data;
    };

}

#endif // PYTHONAPI_PYVARIANT_H
