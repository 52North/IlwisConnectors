#ifndef PYTHONAPI_ILWIS_H
#define PYTHONAPI_ILWIS_H

#include <stdexcept>

//predeclaration of PyObject from object.h
typedef struct _object PyObject;

namespace pythonapi{
    //init modul (ilwis.cpp)
    bool initIlwisObjects();
    //can retrieve Ilwis::ErrorObject::message (ilwis.cpp)
    const char* get_err_message(std::exception& e);
    //returns typeid(Ilwis::ErrorObject) to compare in translate_Exception_type (ilwis.cpp)
    size_t ilwisErrorObject_type_info();
    //invokes PySys_WriteStderr() call (py_error.cpp)
    void log(const char* message);
    //new native Python Exception object (py_error.cpp)
    extern PyObject* ilwisException;
    //tries to translate stdexcept's into native Python Exceptions (py_error.cpp)
    PyObject* translate_Exception_type(std::exception& e);
}
#endif // PYTHONAPI_ILWIS_H
