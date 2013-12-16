#include <QVariant>
#include "pythonapi_pyvariant.h"

#include "../../IlwisCore/core/ilwis.h"
#include "../../IlwisCore/core/errorobject.h"

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

PyVariant::PyVariant(const char *value): PyVariant(new QVariant(value)){
}

PyVariant::~PyVariant(){
}

void PyVariant::__del__(){
    this->~PyVariant();
}

const char* PyVariant::__str__(){
    return this->_data->toString().toLocal8Bit();
}

qlonglong PyVariant::__int__(){
    if(!this->_data->canConvert(QVariant::LongLong))
        throw std::domain_error(QString("PyVariant cannot convert '%1' to int").arg(this->_data->toString()).toStdString());
    else{
        bool ok = false;
        qlonglong ret = this->_data->toLongLong(&ok);
        if (!ok)
            throw std::domain_error(QString("PyVariant cannot convert '%1' to int(qlonglong)").arg(this->_data->toString()).toStdString());
        return ret;
    }
}

double PyVariant::__float__(){
    if(!this->_data->canConvert(QVariant::Double))
        throw std::domain_error(QString("PyVariant cannot convert '%1' to float").arg(this->_data->toString()).toStdString());
    else{
        bool ok = false;
        double ret = this->_data->toDouble(&ok);
        if (!ok)
            throw std::domain_error(QString("PyVariant cannot convert '%1' to float").arg(this->_data->toString()).toStdString());
        return ret;
    }
}

bool PyVariant::__bool__() const{
    if(!this->_data->canConvert(QVariant::Bool))
        throw std::domain_error(QString("PyVariant cannot convert '%1' to bool").arg(this->_data->toString()).toStdString());
    else{
        return this->_data->toBool();
    }
}

IlwisTypes PyVariant::ilwisType(){
    return itANY;
}

QVariant& PyVariant::data(){
    return (*this->_data.get());
}

PyVariant* PyVariant::toPyVariant(Object* obj){
    PyVariant* ptr = static_cast<PyVariant*>(obj);
    if(!ptr)
        throw Ilwis::ErrorObject(QString("cast to PyVariant not possible"));
    return ptr;
}
