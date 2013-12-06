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

PyVariant::PyVariant(int value): PyVariant(new QVariant(value)){
}

PyVariant::PyVariant(uint value): PyVariant(new QVariant(value)){
}

PyVariant::PyVariant(qlonglong value): PyVariant(new QVariant(value)){
}

PyVariant::PyVariant(qulonglong value): PyVariant(new QVariant(value)){
}

PyVariant::PyVariant(bool value): PyVariant(new QVariant(value)){
}

PyVariant::PyVariant(double value): PyVariant(new QVariant(value)){
}

PyVariant::PyVariant(float value): PyVariant(new QVariant(value)){
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
        int ret = this->_data->toLongLong(&ok);
        if (!ok)
            throw std::domain_error(QString("PyVariant cannot convert '%1' to int").arg(this->_data->toString()).toStdString());
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
    return (bool)this->_data && !this->_data->isNull();
}

IlwisTypes PyVariant::ilwisType(){
    return itANY;
}

QVariant *PyVariant::clone(){
    return new QVariant(*this->_data);
}

PyVariant* PyVariant::toPyVariant(Object* obj){
    PyVariant* ptr = static_cast<PyVariant*>(obj);
    if(!ptr)
        throw Ilwis::ErrorObject(QString("cast to PyVariant not possible"));
    return ptr;
}
