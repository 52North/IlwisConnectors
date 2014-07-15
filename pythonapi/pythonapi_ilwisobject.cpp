#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "pythonapi_ilwisobject.h"
#include "pythonapi_error.h"

using namespace pythonapi;

IlwisObject::IlwisObject(){
}

IlwisObject::IlwisObject(Ilwis::IIlwisObject *object): _ilwisObject(std::shared_ptr<Ilwis::IIlwisObject>(object)){
}

IlwisObject::~IlwisObject(){
}

void IlwisObject::setInputConnection(const std::string& url, const std::string& format, const std::string& fnamespace){
    (*this->ptr())->connectTo(QUrl(url.c_str()), QString::fromStdString(format), QString::fromStdString(fnamespace), Ilwis::IlwisObject::ConnectorMode::cmINPUT);
}

void IlwisObject::setOutputConnection(const std::string& url, const std::string& format, const std::string& fnamespace){
    (*this->ptr())->connectTo(QUrl(url.c_str()), QString::fromStdString(format), QString::fromStdString(fnamespace), Ilwis::IlwisObject::ConnectorMode::cmOUTPUT);
}

void IlwisObject::store(int storeMode){
    if (!(*this->ptr())->store(storeMode))
        throw OSError(std::string("IOError on attempt to store ")+this->name());
}

bool IlwisObject::__bool__() const{
    if(this->_ilwisObject != NULL && this->_ilwisObject->isValid()){
        if((*this->_ilwisObject)->ilwisType() != itITEMDOMAIN){
            return (*this->_ilwisObject)->isValid();
        } else {
            return true;
        }
    }
    return false;
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
    (*this->ptr())->name(QString::fromStdString(name));
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

std::string IlwisObject::type2Name(IlwisTypes ilwType){
    if (this->__bool__())
        return Ilwis::IlwisObject::type2Name(ilwType).toStdString();
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
