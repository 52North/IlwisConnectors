#include "Python.h"

#include "pythonapi_ilwis.h"

#include <typeinfo>

void pythonapi::log(const char *message){
    PySys_WriteStderr("ILWIS ERROR: %s \n",message);
}

PyObject* pythonapi::ilwisException;

PyObject* pythonapi::translate_Exception_type(std::exception& e){
    if (typeid(e) == typeid(std::domain_error)){
        return PyExc_TypeError;
    }else if(typeid(e).hash_code() == ilwisErrorObject_type_info()){
        return ilwisException;
    }
    return PyExc_Exception;
}
