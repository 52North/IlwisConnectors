#include "pythonapi_vector.h"

#include "Python.h"

namespace pythonapi {

PyObject* newPyList(int size){
    PyObject *lst = PyList_New(size);
    if (!lst)
        return NULL;
    return lst;
}

bool setListItem(PyObject *list, int i, const char *value){
    PyObject *v = PyUnicode_FromString(value);
    if (!v) {
        Py_DECREF(list);
        list = NULL;
        return false;
    }
    PyList_SET_ITEM(list, i, v);   // reference to str stolen
    return true;
}

bool setListItem(PyObject *list, int i, int value){
    PyObject *v = PyLong_FromSize_t(value);
    if (!v) {
        Py_DECREF(list);
        list = NULL;
        return false;
    }
    PyList_SET_ITEM(list, i, v);   // reference to str stolen
    return true;
}

bool setListItem(PyObject *list, int i, double value){
    PyObject *v = PyFloat_FromDouble(value);
    if (!v) {
        Py_DECREF(list);
        list = NULL;
        return false;
    }
    PyList_SET_ITEM(list, i, v);   // reference to str stolen
    return true;
}

} // namespace pythonapi
