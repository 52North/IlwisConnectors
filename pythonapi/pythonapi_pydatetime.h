#ifndef PYTHONAPI_PYDATETIME_H
#define PYTHONAPI_PYDATETIME_H

//from Python.h
typedef struct _object PyObject;

namespace pythonapi {

    PyObject* PyDateFromDate(int year, int month, int day);
    //    Return a datetime.date object with the specified year, month and day.

    PyObject* PyDateTimeFromDateAndTime(int year, int month, int day, int hour, int minute, int second, int usecond);
    //    Return a datetime.datetime object with the specified year, month, day, hour, minute, second and microsecond.

    PyObject* PyTimeFromTime(int hour, int minute, int second, int usecond);
    //    Return a datetime.time object with the specified hour, minute, second and microsecond.

    PyObject* PyDeltaFromDSU(int days, int seconds, int useconds);
    //    Return a datetime.timedelta object representing the given number of days, seconds and microseconds. Normalization is performed so that the resulting number of microseconds and seconds lie in the ranges documented for datetime.timedelta objects.

    int PyDateCheckExact(const PyObject* ob); // c_type(o) = PyDateTime_Date
    int PyDateTimeCheckExact(const PyObject* ob); // c_type(o) = PyDateTime_DateTime(PyDateTime_Date)
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

#endif // PYTHONAPI_PYDATETIME_H
