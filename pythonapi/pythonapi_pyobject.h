#ifndef PYTHONAPI_PYOBJECT_H
#define PYTHONAPI_PYOBJECT_H

#include <vector>
#include <string>
#include <limits>

typedef struct _object PyObject;
typedef struct bufferinfo Py_buffer;

#include "pythonapi_qtGNUTypedefs.h"

namespace pythonapi {
    const double rUNDEF = -1e308;
    const long   iUNDEF  = -2147483647L;
    const long long i64UNDEF = std::numeric_limits < qint64 >::min();

    const char* typeName(const PyObject* obj);

    //=======std::exception->PyError==========
    void log(std::string message);
    extern PyObject* ilwisException;
    extern PyObject* invalidObjectException;
    extern PyObject* featureCreationError;
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

    //-------------------------------------------
    double CppFloat2Double(const PyObject* ob);
    long    CppLong2long(PyObject *ob);
    unsigned long long CppULongLong2ulonglong(PyObject* ob);
    std::string CppString2stdString(PyObject* ob);
    double CppTupleElement2Double(PyObject* ob, int index);
    long CppTupleElement2Long(PyObject* ob, int index);
    unsigned long long CppTupleElement2ulonglong(PyObject* ob, int index);
    std::string CppTupleElement2String(PyObject* ob, int index);
    int CppTupleElementCount(PyObject* ob);

} // namespace pythonapi

#endif // PYTHONAPI_PYOBJECT_H
