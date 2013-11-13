#ifndef PYTHONAPI_PYVARIANT_H
#define PYTHONAPI_PYVARIANT_H

#include "object.h"
#include <memory>

//Qt typedefs
typedef unsigned int uint;
typedef qint64 qlonglong;
typedef quint64 qulonglong;

class QVariant;

namespace pythonapi{

    class PyVariant: public Object{
    public:
        PyVariant();
        PyVariant(QVariant* data);

        //QVartiant constructor wrapper
        PyVariant(int value);
        PyVariant(uint value);
        PyVariant(qlonglong value);
        PyVariant(qulonglong value);
        PyVariant(bool value);
        PyVariant(double value);
        PyVariant(float value);
        PyVariant(const char* value);

        ~PyVariant();
        void __del__();
        const char* __str__();
        int __int__();
        bool __bool__() const;
        IlwisTypes ilwisType();

        QVariant* clone();

        /**
        * \brief toPyVariant provides static_cast for the return Value of the Engine.do() method on Python side
        * \param obj takes the return value of Engine::_do() which is of the general type pythonapi::Object
        * \return castet pointer to the same object as PyVariant*
        */
        static PyVariant* toPyVariant(Object* obj);


    protected:
        std::unique_ptr<QVariant> _data;
    };

}

#endif // PYTHONAPI_PYVARIANT_H
