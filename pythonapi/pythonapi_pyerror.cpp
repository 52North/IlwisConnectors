#include "Python.h"

#include "pythonapi_ilwis.h"

#include "geos/io/ParseException.h"

#include <typeinfo>

void pythonapi::log(std::string message){
    PySys_WriteStderr("ILWIS %s \n",message.c_str());
}

PyObject* pythonapi::ilwisException;
PyObject* pythonapi::invalidObjectException;

PyObject* pythonapi::translate_Exception_type(std::exception& e){
    if (typeid(e) == typeid(std::domain_error)){
        return PyExc_ValueError;
    }else if (typeid(e) == typeid(std::out_of_range)){
        return PyExc_IndexError;
    }else if (typeid(e) == typeid(std::bad_alloc)){
        return PyExc_MemoryError;
    }else if (typeid(e) == typeid(std::bad_cast)){
        return invalidObjectException;
    //TODO detect FeatureCreationError and raise translated Python FeatureCreationError
    }else if(typeid(e).hash_code() == ilwisErrorObject_type_info()){
        return ilwisException;
    }else if (typeid(e) == typeid(InvalidObject)){
        return invalidObjectException;
    }else if (typeid(e) == typeid(NotImplementedError)){
        return PyExc_NotImplementedError;
    }else if (typeid(e) == typeid(ImportError)){
        return PyExc_ImportError;
    }else if (typeid(e) == typeid(OSError)){
        return PyExc_OSError;
    }else if(typeid(e) == typeid(StopIteration)){
        return PyExc_StopIteration;
    }else if(typeid(e) == typeid(geos::io::ParseException)){
        return PyExc_SyntaxError;
    }
    return PyExc_Exception;
}

