#ifndef PYTHONAPI_PYVARIANT_H
#define PYTHONAPI_PYVARIANT_H

#include "pythonapi_object.h"
#include <memory>

//Qt typedefs
typedef unsigned int uint;
typedef qint64 qlonglong;

class QVariant;

namespace pythonapi{

    class PyVariant: public Object{
        friend class Feature;
        friend class PixelIterator;
        friend class Engine;
        friend class Table;
    public:
        PyVariant();
        PyVariant(PyVariant& pv);

        bool isValid();

        PyVariant(qlonglong value);
        PyVariant(double value);
        PyVariant(std::string value);

        ~PyVariant();
        void __del__();
        std::string __str__();
        qlonglong __int__();
        double __float__();
        bool __bool__() const;
        IlwisTypes ilwisType();

        /**
        * \brief toPyVariant provides static_cast for the return Value of the Engine.do() method on Python side
        * \param obj takes the return value of Engine::_do() which is of the general type pythonapi::Object
        * \return castet pointer to the same object as PyVariant*
        */
        static PyVariant* toPyVariant(Object* obj);


    private:
        PyVariant(QVariant* data);//takes ownership of this QVariant instance
        QVariant& data();
        std::unique_ptr<QVariant> _data;
    };

}

#endif // PYTHONAPI_PYVARIANT_H
