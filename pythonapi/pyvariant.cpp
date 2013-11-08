#include <QVariant>
#include "pyvariant.h"

#include "../../IlwisCore/core/errorobject.h"

using namespace pythonapi;

PyVariant::PyVariant(): _data(std::unique_ptr<QVariant>(new QVariant())){
}

PyVariant::PyVariant(QVariant* data): _data(std::unique_ptr<QVariant>(data)){
}

PyVariant::~PyVariant(){
}

void PyVariant::__del__(){
    this->~PyVariant();
}

const char* PyVariant::__str__(){
    return this->_data->toString().toLocal8Bit();
}

int PyVariant::__int__(){
    if(!this->_data->canConvert(QVariant::Int))
        throw Ilwis::ErrorObject(QString("QVariant cannot convert '%1' to int").arg(this->_data->toString()));
    else{
        bool ok = false;
        int ret = this->_data->toInt(&ok);
        if (!ok)
            throw std::domain_error(QString("QVariant cannot convert '%1' to int").arg(this->_data->toString()).toStdString());
        return ret;
    }
}

bool PyVariant::__bool__() const{
    return (bool)this->_data && !this->_data->isNull();
}
