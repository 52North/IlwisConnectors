#ifndef PYTHONAPI_ILWIS_H
#define PYTHONAPI_ILWIS_H

#include <stdexcept>
#include <limits>

#include "pythonapi_qtGNUTypedefs.h"

//predeclaration of PyObject from object.h
typedef struct _object PyObject;

namespace pythonapi{

    //to indicate an error caused by the attempt to use an invalid (ilwis) object
    class InvalidObject : public std::exception{
    private:
        std::string _what;
    public:
        InvalidObject(const std::string& what ){
            this->_what = what;
        }
        virtual const char* what() const throw(){
            return this->_what.c_str();
        }
    };

    //to indicate an error during module initialization
    class ImportError : public std::exception{
    private:
        std::string _what;
    public:
        ImportError(const std::string& what ){
            this->_what = what;
        }
        virtual const char* what() const throw(){
            return this->_what.c_str();
        }
    };

    //to indicate an error system interaction (like IO error)
    class OSError : public std::exception{
    private:
        std::string _what;
    public:
        OSError(const std::string& what ){
            this->_what = what;
        }
        virtual const char* what() const throw(){
            return this->_what.c_str();
        }
    };

    //to indicate that this ilwis functionality is not yet implemented
    class NotImplementedError : public std::exception{
    private:
        std::string _what;
    public:
        NotImplementedError(const std::string& what ){
            this->_what = what;
        }
        virtual const char* what() const throw(){
            return this->_what.c_str();
        }
    };

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
    //can retrieve Ilwis::ErrorObject::message() or std::exception::what() (ilwis.cpp)
    const char *get_err_message(std::exception& e);
    //returns typeid(Ilwis::ErrorObject) to compare in translate_Exception_type (ilwis.cpp)
    size_t ilwisErrorObject_type_info();
    //invokes PySys_WriteStderr() call (pythonapi_pyerror.cpp)
    void log(std::string message);
    //new native Python Exception objects (pythonapi_pyerror.cpp)
    extern PyObject* ilwisException;
    extern PyObject* invalidObjectException;
    //tries to translate stdexcept's into native Python Exceptions (pythonapi_pyerror.cpp)
    PyObject* translate_Exception_type(std::exception& e);
}
#endif // PYTHONAPI_ILWIS_H
