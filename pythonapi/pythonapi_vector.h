#ifndef PYTHONAPI_VECTOR_H
#define PYTHONAPI_VECTOR_H

#include <vector>

typedef struct _object PyObject;

namespace pythonapi {

    PyObject* newPyList(int size);
    bool setListItem(PyObject* list, int i, const char* value);
    bool setListItem(PyObject* list, int i, int value);
    bool setListItem(PyObject *list, int i, double value);

} // namespace pythonapi

#endif // PYTHONAPI_VECTOR_H
