#ifndef PYTHONAPI_PYOBJECT_H
#define PYTHONAPI_PYOBJECT_H

#include <vector>
#include <string>

typedef struct _object PyObject;
typedef struct bufferinfo Py_buffer;

#include "pythonapi_qtGNUTypedefs.h"

namespace pythonapi {

    const char* typeName(const PyObject* obj);

    //=======std::exception->PyError==========
    void log(std::string message);
    extern PyObject* ilwisException;
    extern PyObject* invalidObjectException;
    PyObject* translate_Exception_type(std::exception& e);

    //=======basic types=======================
    PyObject* PyFloatFromDouble(double v);
    PyObject* PyLongFromLongLong(long long v);
    PyObject* PyLongFromUnsignedLongLong(unsigned long long v);
    PyObject* PyUnicodeFromString(const char *u);
    PyObject* PyBoolFromLong(long v);
    PyObject* PyLongFromSize_t(quint32 v);

    //======tuple==========================
    PyObject* newPyTuple(int size);
    bool setTupleItem(PyObject *tuple, int i, PyObject* v);

    //========Py_buffer==========================
    Py_buffer* newPyBuffer(void* buf, int len, int readOnly);

    //=========datetime================================
    //    Return a datetime.date object with the specified year, month and day.
    PyObject* PyDateFromDate(int year, int month, int day);

    //    Return a datetime.datetime object with the specified year, month, day, hour, minute, second and microsecond.
    PyObject* PyDateTimeFromDateAndTime(int year, int month, int day, int hour, int minute, int second, int usecond);

    //    Return a datetime.time object with the specified hour, minute, second and microsecond.
    PyObject* PyTimeFromTime(int hour, int minute, int second, int usecond);

    //    Return a datetime.timedelta object representing the given number of days, seconds and microseconds. Normalization is performed so that the resulting number of microseconds and seconds lie in the ranges documented for datetime.timedelta objects.
    PyObject* PyDeltaFromDSU(int days, int seconds, int useconds);

    int PyDateCheckExact(const PyObject* ob); // c_type(o) = PyDateTime_Date
    int PyDateTimeCheckExact(const PyObject* ob); // c_type(o) =               PyDateTime_DateTime(PyDateTime_Date)
    int PyTimeCheckExact(const PyObject* ob); // c_type(o) = PyDateTime_Time

    // c_type(o) = PyDateTime_Date | PyDateTime_DateTime
    int PyDateTimeGET_YEAR(const void* o);
    int PyDateTimeGET_MONTH(const void* o);
    int PyDateTimeGET_DAY(const void* o);

    // c_type(o) = PyDateTime_DateTime
    int PyDateTimeDATE_GET_HOUR(const void* o);
    int PyDateTimeDATE_GET_MINUTE(const void* o);
    int PyDateTimeDATE_GET_SECOND(const void* o);
    int PyDateTimeDATE_GET_MICROSECOND(const void* o);

    // c_type(o) = PyDateTime_Time
    int PyDateTimeTIME_GET_HOUR(const void* o);
    int PyDateTimeTIME_GET_MINUTE(const void* o);
    int PyDateTimeTIME_GET_SECOND(const void* o);
    int PyDateTimeTIME_GET_MICROSECOND(const void* o);

} // namespace pythonapi

#endif // PYTHONAPI_PYOBJECT_H
