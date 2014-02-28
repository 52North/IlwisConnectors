#ifndef PYTHONAPI_PYVARIANT_H
#define PYTHONAPI_PYVARIANT_H

#include "pythonapi_object.h"
#include <memory>

//Qt typedefs
typedef unsigned int uint;
typedef qint64 qlonglong;
typedef quint64 qulonglong;

class QVariant;

namespace pythonapi{

    QVariant* PyObject2QVariant(const PyObject* obj);
    PyObject* QVariant2PyObject(const QVariant& var);

}

#endif // PYTHONAPI_PYVARIANT_H
