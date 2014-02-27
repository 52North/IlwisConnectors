#include <QVariant>
#include <QMetaType>
#include "pythonapi_pyvariant.h"

#include "../../IlwisCore/core/ilwis.h"
#include "../../IlwisCore/core/errorobject.h"

#include "pythonapi_pydatetime.h"
#include "pythonapi_pycontainer.h"
#include <time.h>
#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/util/range.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericrange.h"
#include "../../IlwisCore/core/util/juliantime.h"
using namespace pythonapi;

PyVariant::PyVariant(): _data(std::unique_ptr<QVariant>(new QVariant())){
}

PyVariant::PyVariant(PyVariant &pv): _data(std::unique_ptr<QVariant>(new QVariant((*pv._data)))){
}

PyVariant::PyVariant(QVariant* data): _data(std::unique_ptr<QVariant>(data)){
}

bool PyVariant::isValid(){
    return (bool)this->_data && !this->_data->isNull();
}

PyVariant::PyVariant(qlonglong value): PyVariant(new QVariant(value)){
}

PyVariant::PyVariant(double value): PyVariant(new QVariant(value)){
}

PyVariant::PyVariant(std::string value): PyVariant(new QVariant(value.c_str())){
}

PyVariant::~PyVariant(){
}

void PyVariant::__del__(){
    this->~PyVariant();
}

std::string PyVariant::__str__(){
    return this->_data->toString().toStdString();
}

qlonglong PyVariant::__int__(){
    if(!this->_data->canConvert(QVariant::LongLong))
        throw std::domain_error(QString("Cannot convert '%1' to int").arg(this->_data->toString()).toStdString());
    else{
        bool ok = false;
        qlonglong ret = this->_data->toLongLong(&ok);
        if (!ok)
            throw std::domain_error(QString("Cannot convert '%1' to int(qlonglong)").arg(this->_data->toString()).toStdString());
        return ret;
    }
}

double PyVariant::__float__(){
    if(!this->_data->canConvert(QVariant::Double))
        throw std::domain_error(QString("Cannot convert '%1' to float").arg(this->_data->toString()).toStdString());
    else{
        bool ok = false;
        double ret = this->_data->toDouble(&ok);
        if (!ok)
            throw std::domain_error(QString("Cannot convert '%1' to float").arg(this->_data->toString()).toStdString());
        return ret;
    }
}

bool PyVariant::__bool__() const{
    if(!this->_data->canConvert(QVariant::Bool))
        throw std::domain_error(QString("Cannot convert '%1' to bool").arg(this->_data->toString()).toStdString());
    else{
        return this->_data->toBool();
    }
}

PyObject* PyVariant::toDateTime() const{
    if ( QString(this->_data->typeName()).compare("Ilwis::Time") != 0){
        throw std::domain_error(QString("Cannot convert '%1' to datetime").arg(this->_data->toString()).toStdString());
    }else{
        Ilwis::Time time = this->_data->value<Ilwis::Time>();
        return PyDateTimeFromDateAndTime(
                    time.get(Ilwis::Time::tpYEAR),
                    time.get(Ilwis::Time::tpMONTH),
                    time.get(Ilwis::Time::tpDAYOFMONTH),
                    time.get(Ilwis::Time::tpHOUR),
                    time.get(Ilwis::Time::tpMINUTE),
                    time.get(Ilwis::Time::tpSECOND),
                    0 //milliseconds
                );
    }
}

IlwisTypes PyVariant::ilwisType(){
    return itANY;
}

QVariant& PyVariant::data(){
    return (*this->_data.get());
}

QVariant* PyVariant::toQVariant(const PyObject* obj){
    if (obj){
        if(PyDateCheckExact(obj)){
            Ilwis::Time time;
            return new QVariant(IVARIANT(time));
        }else if(PyTimeCheckExact(obj)){
            Ilwis::Time time;
            return new QVariant(IVARIANT(time));
        }else if(PyDateTimeCheckExact(obj)){
            Ilwis::Time time;
            return new QVariant(IVARIANT(time));
        }
        throw std::domain_error(QString("Cannot convert PyObject of type %1").arg(typeName(obj)).toStdString());
    }
    return new QVariant();
}

PyObject* pythonapi::PyVariant::toPyObject(const QVariant& var){
    bool ok = false;
    QMetaType::Type t = (QMetaType::Type)var.type();
    if (t == QMetaType::QString){
        return PyUnicodeFromString(var.toString().toLocal8Bit());
    }else if(t == QMetaType::Bool){
        return PyBoolFromLong(var.toBool());
    }else if(t == QMetaType::Double){
        double ret = var.toDouble(&ok);
        if (!ok)
            throw std::domain_error(QString("Cannot convert '%1' to float").arg(var.toString()).toStdString());
        return PyFloatFromDouble(ret);
    }else if((t == QMetaType::LongLong) || (t == QMetaType::Long) || (t == QMetaType::ULong) || (t == QMetaType::Int) || (t == QMetaType::UInt)){
        qlonglong ret = var.toLongLong(&ok);
        if (!ok)
            throw std::domain_error(QString("Cannot convert '%1' to int(qlonglong)").arg(var.toString()).toStdString());
        return PyLongFromLong(ret);
    }else if(t == QMetaType::ULongLong){
        qulonglong ret = var.toULongLong(&ok);
        if (!ok)
            throw std::domain_error(QString("Cannot convert '%1' to int(qulonglong)").arg(var.toString()).toStdString());
        return PyLongFromUnsignedLong(ret);
    }else if(QString(var.typeName()).compare("Ilwis::Time") == 0){
        if (var.canConvert<Ilwis::Time>()){
            Ilwis::Time time = var.value<Ilwis::Time>();
            switch(time.valueType()){
                case itTIME: return PyTimeFromTime(
                            time.get(Ilwis::Time::tpHOUR),
                            time.get(Ilwis::Time::tpMINUTE),
                            time.get(Ilwis::Time::tpSECOND),
                            0 //milliseconds
                        );
                case itDATE: return PyDateFromDate(
                            time.get(Ilwis::Time::tpYEAR),
                            time.get(Ilwis::Time::tpMONTH),
                            time.get(Ilwis::Time::tpDAYOFMONTH)
                        );
                case itDATETIME: return PyDateTimeFromDateAndTime(
                            time.get(Ilwis::Time::tpYEAR),
                            time.get(Ilwis::Time::tpMONTH),
                            time.get(Ilwis::Time::tpDAYOFMONTH),
                            time.get(Ilwis::Time::tpHOUR),
                            time.get(Ilwis::Time::tpMINUTE),
                            time.get(Ilwis::Time::tpSECOND),
                            0 //milliseconds
                        );
                throw std::domain_error(QString("Cannot convert Ilwis::Time object to datetime").toStdString());
            }
        }else{
            throw std::domain_error(QString("Cannot convert Ilwis::Time object to datetime").toStdString());
        }
    }
    throw std::domain_error(QString("Cannot convert: %1 of type %2").arg(var.toString()).arg(QMetaType::typeName(t)).toStdString());
}
