#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "pythonapi_ilwisobject.h"

using namespace pythonapi;

IlwisObject::IlwisObject(){
}

IlwisObject::IlwisObject(Ilwis::IIlwisObject *object): _ilwisObject(std::shared_ptr<Ilwis::IIlwisObject>(object)){
}

IlwisObject::~IlwisObject(){
}

void IlwisObject::setConnection(const std::string& url, const std::string& format, const std::string& fnamespace, ConnectorMode cmode){
    Ilwis::IIlwisObject io = (*this->ptr());
    if (cmode == cmINPUT || cmode == cmEXTENDED){
        if (!io.prepare(QString::fromStdString(url), io->ilwisType()))
            throw Ilwis::ErrorObject(QString("Cannot reconnect %1 for input").arg(url.c_str()));
    }else{
        io->connectTo(QUrl(url.c_str()), QString::fromStdString(format), QString::fromStdString(fnamespace), (Ilwis::IlwisObject::ConnectorMode)cmode);
    }
}

void IlwisObject::store(int storeMode){
    if (!(*this->ptr())->store(storeMode))
        throw OSError(std::string("IOError on attempt to store ")+this->name());
}

bool IlwisObject::__bool__() const{
    return this->_ilwisObject != NULL && this->_ilwisObject->isValid() && (*this->_ilwisObject)->isValid();
}

std::string IlwisObject::__str__(){
    if (this->__bool__())
        return QString("%1%2").arg(NAME_ALIAS).arg((*this->ptr())->id()).toStdString();
    else
        return  std::string("invalid IlwisObject!");
}

std::string IlwisObject::name(){
    if (this->__bool__())
        return (*this->ptr())->name().toStdString();
    else
        return  std::string("invalid IlwisObject!");
}

void IlwisObject::name(std::string name){
    (*this->ptr())->setName(QString::fromStdString(name));
}

bool IlwisObject::isInternal() const{
    return (*this->ptr())->isInternalObject();
}

std::string IlwisObject::type(){
    if (this->__bool__())
        return Ilwis::IlwisObject::type2Name((*this->ptr())->ilwisType()).toStdString();
    else
        return  std::string("invalid IlwisObject!");
}

std::string IlwisObject::__add__(std::string value){
    if (this->__bool__())
        return this->__str__()+value;
    else
        return std::string("invalid IlwisObject!");
}

std::string IlwisObject::__radd__(std::string value){
    if (this->__bool__())
        return value+this->__str__();
    else
        return std::string("invalid IlwisObject!");
}

std::shared_ptr<Ilwis::IIlwisObject> IlwisObject::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("invalid IlwisObject");
    return this->_ilwisObject;
}

quint64 IlwisObject::ilwisID() const{
    return (*this->ptr())->id();
}

IlwisTypes IlwisObject::ilwisType(){
    return (*this->ptr())->ilwisType();
}
