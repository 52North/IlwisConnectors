
#include "javaapi_ilwisoperation.h"

using namespace javaapi;

IlwisOperation::IlwisOperation(Ilwis::Resource& res)/*:operation(res)*/ {
    operation = new Ilwis::Resource(res);
}

IlwisOperation::~IlwisOperation() {
    if (operation != 0)
        delete operation;
}

IlwisOperation::IlwisOperation():operation(0) {

}

IlwisOperation::IlwisOperation(const IlwisOperation& other) {
    operation = new Ilwis::Resource(*(other.operation));
}

IlwisOperation& IlwisOperation::operator=(const IlwisOperation& other) {
    if (operation != 0)
        delete operation;
    operation = new Ilwis::Resource(*(other.operation));
    return *this;
}

std::string IlwisOperation::getName() const {
    return operation->name().toStdString();
}

std::string IlwisOperation::getSyntax() const {
    return (*operation)["syntax"].toString().toStdString();
}

std::string IlwisOperation::getLongname() const {
    return (*operation)["longname"].toString().toStdString();
}

std::string IlwisOperation::getKeyword() const {
    return (*operation)["keyword"].toString().toStdString();
}

std::string IlwisOperation::getInparameters() const {
    return (*operation)["inputparameters"].toString().toStdString();
}

std::string IlwisOperation::getOutparameters() const {
    return (*operation)["outputparameters"].toString().toStdString();
}

qint64 IlwisOperation::getId() const {
    return operation->id();
}

bool IlwisOperation::isValid() const {
    return operation->isValid();
}

std::string IlwisOperation::getPinName(qint32 orderNumber) const {
    return (*operation)[ QString("pin_%1_name").arg(orderNumber) ].toString().toStdString();
}

qint64 IlwisOperation::getPinType(qint32 orderNumber) const {
    bool ok;
    return (*operation)[ QString("pin_%1_type").arg(orderNumber) ].toLongLong(&ok);
}

std::string IlwisOperation::getPinDesc(qint32 orderNumber) const {
    return (*operation)[ QString("pin_%1_desc").arg(orderNumber) ].toString().toStdString();
}

std::string IlwisOperation::getPoutName(qint32 orderNumber) const {
    return (*operation)[ QString("pout_%1_name").arg(orderNumber) ].toString().toStdString();
}

qint64 IlwisOperation::getPoutType(qint32 orderNumber) const {
    bool ok;
    return (*operation)[ QString("pout_%1_type").arg(orderNumber) ].toLongLong(&ok);
}

std::string IlwisOperation::getPoutDesc(qint32 orderNumber) const {
    return (*operation)[ QString("pout_%1_desc").arg(orderNumber) ].toString().toStdString();
}
