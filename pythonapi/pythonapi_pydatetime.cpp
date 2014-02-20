#include "Python.h"
#include "datetime.h"
#include "pythonapi_pydatetime.h"

namespace pythonapi {

    PyObject* PyDateFromDate(int year, int month, int day){
        PyDateTime_IMPORT;
        return PyDate_FromDate(year, month, day);
    }

    PyObject* PyDateTimeFromDateAndTime(int year, int month, int day, int hour, int minute, int second, int usecond){
        PyDateTime_IMPORT;
        return PyDateTime_FromDateAndTime(year, month, day, hour, minute, second, usecond);
    }

    PyObject* PyTimeFromTime(int hour, int minute, int second, int usecond){
        PyDateTime_IMPORT;
        return PyTime_FromTime(hour, minute, second, usecond);
    }

    PyObject* PyDeltaFromDSU(int days, int seconds, int useconds){
        PyDateTime_IMPORT;
        return PyDelta_FromDSU(days, seconds, useconds);
    }

} // namespace pythonapi
