#ifndef PYTHONAPI_PYDATETIME_H
#define PYTHONAPI_PYDATETIME_H

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

} // namespace pythonapi

#endif // PYTHONAPI_PYDATETIME_H
