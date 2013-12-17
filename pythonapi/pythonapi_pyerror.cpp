#include "Python.h"

#include "pythonapi_ilwis.h"

#include <typeinfo>

void pythonapi::log(const char *message){
    PySys_WriteStderr("ILWIS ERROR: %s \n",message);
}

PyObject* pythonapi::ilwisException;
PyObject* pythonapi::wrapperException;
PyObject* pythonapi::invalidObjectException;

PyObject* pythonapi::translate_Exception_type(std::exception& e){
    if (typeid(e) == typeid(std::domain_error)){
        return PyExc_TypeError;
    }else if (typeid(e) == typeid(std::out_of_range)){
        return PyExc_IndexError;
    }else if (typeid(e) == typeid(std::bad_alloc)){
        return PyExc_MemoryError;
    }else if (typeid(e) == typeid(std::bad_cast)){
        return invalidObjectException;
    }else if(typeid(e).hash_code() == ilwisErrorObject_type_info()){
        return ilwisException;
    }else if (typeid(e) == typeid(InvalidObject)){
        return invalidObjectException;
    }else if (typeid(e) == typeid(NotImplementedError)){
        return PyExc_NotImplementedError;
    }else if (typeid(e) == typeid(WrapperError)){
        return wrapperException;
    }else if (typeid(e) == typeid(ImportError)){
        return PyExc_ImportError;
    }else if(typeid(e) == typeid(StopIteration)){
        return PyExc_StopIteration;
    }
    return PyExc_Exception;
}

