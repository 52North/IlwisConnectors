#include "pythonapi_error.h"

#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/errorobject.h"


namespace pythonapi {

    InvalidObject::InvalidObject(const std::string& what ){
        this->_what = what;
    }

    const char *InvalidObject::what() const throw(){
        return this->_what.c_str();
    }

    ImportError::ImportError(const std::string &what){
        this->_what = what;
    }

    const char *ImportError::what() const throw(){
        return this->_what.c_str();
    }

    OSError::OSError(const std::string &what){
        this->_what = what;
    }

    const char *OSError::what() const throw(){
        return this->_what.c_str();
    }

    NotImplementedError::NotImplementedError(const std::string &what){
        this->_what = what;
    }

    const char *NotImplementedError::what() const throw(){
        return this->_what.c_str();
    }

    const char* get_err_message(std::exception &e){
        Ilwis::ErrorObject* err = dynamic_cast<Ilwis::ErrorObject*>(&e);
        if (err)
            return QString("ILWIS %1: %2").arg(err->what(),err->message()).toLocal8Bit();
        else
            return e.what();
    }

    size_t ilwisErrorObject_type_info(){
        return typeid(Ilwis::ErrorObject).hash_code();
    }

}//namespace pythonapi

