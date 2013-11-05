#ifndef PYTHONAPI_EXCEPTION_H
#define PYTHONAPI_EXCEPTION_H

#include <exception>

namespace pythonapi{
    const char* get_err_message(std::exception& e);

    // PyObject* translate_Exception(std::exception& e);
}
#endif // PYTHONAPI_EXCEPTION_H
