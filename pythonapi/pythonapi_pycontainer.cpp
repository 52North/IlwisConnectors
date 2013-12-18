#include "pythonapi_container.h"

#include "Python.h"

namespace pythonapi {

PyObject* newPyTuple(int size){
    return PyTuple_New(size);
}

bool setTupleItem(PyObject *tuple, int i, const char* value){
    PyObject *v = PyUnicode_FromString(value);
    if (!v) {
        Py_DECREF(tuple);
        tuple = NULL;
        return false;
    }
    PyTuple_SET_ITEM(tuple, i, v);   // reference to str stolen
    return true;
}

bool setTupleItem(PyObject *tuple, int i, int value){
    PyObject *v = PyLong_FromSize_t(value);
    if (!v) {
        Py_DECREF(tuple);
        tuple = NULL;
        return false;
    }
    PyTuple_SET_ITEM(tuple, i, v);   // reference to str stolen
    return true;
}

bool setTupleItem(PyObject *tuple, int i, double value){
    PyObject *v = PyFloat_FromDouble(value);
    if (!v) {
        Py_DECREF(tuple);
        tuple = NULL;
        return false;
    }
    PyTuple_SET_ITEM(tuple, i, v);   // reference to str stolen
    return true;
}

} // namespace pythonapi
