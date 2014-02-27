#include "pythonapi_pycontainer.h"

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

bool setTupleItem(PyObject *tuple, int i, const quint32& value){
    PyObject *v = PyLong_FromSize_t(value);
    if (!v) {
        Py_DECREF(tuple);
        tuple = NULL;
        return false;
    }
    PyTuple_SET_ITEM(tuple, i, v);   // reference to v stolen
    return true;
}

bool setTupleItem(PyObject *tuple, int i, const qint64& value){
    PyObject *v = PyLong_FromLong(value);
    if (!v) {
        Py_DECREF(tuple);
        tuple = NULL;
        return false;
    }
    PyTuple_SET_ITEM(tuple, i, v);   // reference to v stolen
    return true;
}

bool setTupleItem(PyObject *tuple, int i, const double& value){
    PyObject *v = PyFloat_FromDouble(value);
    if (!v) {
        Py_DECREF(tuple);
        tuple = NULL;
        return false;
    }
    PyTuple_SET_ITEM(tuple, i, v);   // reference to v stolen
    return true;
}


Py_buffer* newPyBuffer(void* buf, int len, int readOnly){
    Py_buffer* ret =(Py_buffer*)malloc(sizeof *ret);
    if (PyBuffer_FillInfo(ret, NULL,  buf, len, readOnly, PyBUF_WRITEABLE)) {
      throw PyExc_BufferError;
    }
    return ret;
}

PyObject* PyFloatFromDouble(double v){
    return PyFloat_FromDouble(v);
}

PyObject* PyLongFromLong(long v){
    return PyLong_FromLong(v);
}

PyObject* PyLongFromUnsignedLong(long v){
    return PyLong_FromUnsignedLong(v);
}

PyObject* PyUnicodeFromString(const char *u){
    return PyUnicode_FromString(u);
}

PyObject* PyBoolFromLong(long v){
    return PyBool_FromLong(v);
}

const char* typeName(const PyObject* obj){
    return obj->ob_type->tp_name;
}

} // namespace pythonapi
