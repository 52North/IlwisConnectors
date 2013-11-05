#ifndef PYTHONAPI_PYVARIANT_H
#define PYTHONAPI_PYVARIANT_H

class QVariant;

namespace pythonapi{

    class PyVariant{
    public:
        PyVariant();
        PyVariant(QVariant* data);
        ~PyVariant();
        void __del__();
        const char* __str__();
        int __int__();
    protected:
        QVariant* _data;
    };

}

#endif // PYTHONAPI_PYVARIANT_H
