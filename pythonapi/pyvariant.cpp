#include <QVariant>
#include "pyvariant.h"

using namespace pythonapi;

PyVariant::PyVariant(QVariant* data): _data(data){
}

PyVariant::~PyVariant(){
    delete this->_data;
}

const char* PyVariant::__str__(){
    return this->_data->toString().toLocal8Bit();
}


PyVariant::PyVariant(): _data(new QVariant()){
}
