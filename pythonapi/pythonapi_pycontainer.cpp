#include "pythonapi_pycontainer.h"

#include "Python.h"

namespace pythonapi {

PyObject* newPyTuple(int size){
    return PyTuple_New(size);
}

bool setTupleItem(PyObject *tuple, int i, PyObject* v){
    if (v) {
        PyTuple_SET_ITEM(tuple, i, v);   // reference to v stolen
        return true;
    }else{
        Py_DECREF(tuple);
        tuple = NULL;
        return false;
    }
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

PyObject* PyLongFromLongLong(long long v){
    return PyLong_FromLongLong(v);
}

PyObject* PyLongFromUnsignedLongLong(unsigned long long v){
    return PyLong_FromUnsignedLongLong(v);
}

PyObject* PyUnicodeFromString(const char *u){
    return PyUnicode_FromString(u);
}

PyObject* PyBoolFromLong(long v){
    return PyBool_FromLong(v);
}

PyObject* PyLongFromSize_t(quint32 v){
    return PyLong_FromSize_t(v);
}

const char* typeName(const PyObject* obj){
    return obj->ob_type->tp_name;
}

} // namespace pythonapi
