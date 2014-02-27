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

    int PyDateCheckExact(const PyObject* ob);
    int PyDateTimeCheckExact(const PyObject* ob);
    int PyTimeCheckExact(const PyObject* ob);
    int PyDateTimeGET_YEAR(void *o);
    int PyDateTimeGET_MONTH(void *o);
    int PyDateTimeGET_DAY(void *o);
    int PyDateTimeDATE_GET_HOUR(void *o);
    int PyDateTimeDATE_GET_MINUTE(void *o);
    int PyDateTimeDATE_GET_SECOND(void *o);
    int PyDateTimeDATE_GET_MICROSECOND(void *o);
    int PyDateTimeTIME_GET_HOUR(void *o);
    int PyDateTimeTIME_GET_MINUTE(void *o);
    int PyDateTimeTIME_GET_SECOND(void *o);
    int PyDateTimeTIME_GET_MICROSECOND(void *o);


} // namespace pythonapi

#endif // PYTHONAPI_PYDATETIME_H
