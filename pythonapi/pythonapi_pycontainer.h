#ifndef PYTHONAPI_VECTOR_H
#define PYTHONAPI_VECTOR_H

#include <vector>

typedef struct _object PyObject;
typedef struct bufferinfo Py_buffer;

namespace pythonapi {

    PyObject* newPyTuple(int size);
    bool setTupleItem(PyObject* tuple, int i, const char* value);
    bool setTupleItem(PyObject* tuple, int i, int value);
    bool setTupleItem(PyObject *tuple, int i, double value);

    Py_buffer* newPyBuffer(void* buf, int len, int readOnly);

} // namespace pythonapi

#endif // PYTHONAPI_VECTOR_H
