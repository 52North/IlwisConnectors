#ifndef PYTHONAPI_VECTOR_H
#define PYTHONAPI_VECTOR_H

#include <vector>

typedef struct _object PyObject;
typedef struct bufferinfo Py_buffer;

#include "pythonapi_qtGNUTypedefs.h"

namespace pythonapi {

    PyObject* newPyTuple(int size);
    bool setTupleItem(PyObject *tuple, int i, PyObject* v);

    Py_buffer* newPyBuffer(void* buf, int len, int readOnly);

    PyObject* PyFloatFromDouble(double v);
    PyObject* PyLongFromLongLong(long long v);
    PyObject* PyLongFromUnsignedLongLong(unsigned long long v);
    PyObject* PyUnicodeFromString(const char *u);
    PyObject* PyBoolFromLong(long v);
    PyObject* PyLongFromSize_t(quint32 v);

    const char* typeName(const PyObject* obj);

} // namespace pythonapi

#endif // PYTHONAPI_VECTOR_H
