#include "Python.h"
#include "pythonapi_pydatetime.h"

#include "datetime.h"

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

    int PyDateCheckExact(const PyObject* ob){
        return PyDate_CheckExact(const_cast<PyObject*>(ob));
    }

    int PyDateTimeCheckExact(const PyObject* ob){
        return PyDateTime_CheckExact(const_cast<PyObject*>(ob));
    }

    int PyTimeCheckExact(const PyObject* ob){
        return PyTime_CheckExact(const_cast<PyObject*>(ob));
    }

    int PyDateTimeGET_YEAR(const void* o){
        return PyDateTime_GET_YEAR(reinterpret_cast<PyDateTime_Date*>(const_cast<void*>(o)));
    }

    int PyDateTimeGET_MONTH(const void* o){
        return PyDateTime_GET_MONTH(reinterpret_cast<PyDateTime_Date*>(const_cast<void*>(o)));
    }

    int PyDateTimeGET_DAY(const void* o){
        return PyDateTime_GET_DAY(reinterpret_cast<PyDateTime_Date*>(const_cast<void*>(o)));
    }

    int PyDateTimeDATE_GET_HOUR(const void* o){
        return PyDateTime_DATE_GET_HOUR(reinterpret_cast<PyDateTime_DateTime*>(const_cast<void*>(o)));
    }

    int PyDateTimeDATE_GET_MINUTE(const void* o){
        return PyDateTime_DATE_GET_MINUTE(reinterpret_cast<PyDateTime_DateTime*>(const_cast<void*>(o)));
    }

    int PyDateTimeDATE_GET_SECOND(const void* o){
        return PyDateTime_DATE_GET_SECOND(reinterpret_cast<PyDateTime_DateTime*>(const_cast<void*>(o)));
    }

    int PyDateTimeDATE_GET_MICROSECOND(const void* o){
        return PyDateTime_DATE_GET_MICROSECOND(reinterpret_cast<PyDateTime_DateTime*>(const_cast<void*>(o)));
    }

    int PyDateTimeTIME_GET_HOUR(const void* o){
        return PyDateTime_TIME_GET_HOUR(reinterpret_cast<PyDateTime_Time*>(const_cast<void*>(o)));
    }

    int PyDateTimeTIME_GET_MINUTE(const void* o){
        return PyDateTime_TIME_GET_MINUTE(reinterpret_cast<PyDateTime_Time*>(const_cast<void*>(o)));
    }

    int PyDateTimeTIME_GET_SECOND(const void* o){
        return PyDateTime_TIME_GET_SECOND(reinterpret_cast<PyDateTime_Time*>(const_cast<void*>(o)));
    }

    int PyDateTimeTIME_GET_MICROSECOND(const void* o){
        return PyDateTime_TIME_GET_MICROSECOND(reinterpret_cast<PyDateTime_Time*>(const_cast<void*>(o)));
    }



} // namespace pythonapi
