#ifndef PYTHONAPI_ILWIS_H
#define PYTHONAPI_ILWIS_H

#include <stdexcept>
#include <limits>

#include "pythonapi_qtGNUTypedefs.h"

//predeclaration of PyObject from object.h
typedef struct _object PyObject;

namespace pythonapi{

    //to indicate next() == it.end() the Python way
    class StopIteration : public std::exception{
    public:
        StopIteration(){}
        virtual const char* what() const throw() {return "StopIteration";}
    };

    //init modul (ilwis.cpp)
    bool initIlwisObjects();
    void disconnectIssueLogger(); // (ilwis.cpp)
    void connectIssueLogger();
    //can retrieve Ilwis::ErrorObject::message (ilwis.cpp)
    const char* get_err_message(std::exception& e);
    //returns typeid(Ilwis::ErrorObject) to compare in translate_Exception_type (ilwis.cpp)
    size_t ilwisErrorObject_type_info();
    //invokes PySys_WriteStderr() call (pythonapi_pyerror.cpp)
    void log(const char* message);
    //new native Python Exception object (pythonapi_pyerror.cpp)
    extern PyObject* ilwisException;
    //tries to translate stdexcept's into native Python Exceptions (pythonapi_pyerror.cpp)
    PyObject* translate_Exception_type(std::exception& e);
}
#endif // PYTHONAPI_ILWIS_H
