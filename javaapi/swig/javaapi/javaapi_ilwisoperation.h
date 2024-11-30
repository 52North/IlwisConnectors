#ifndef JAVAAPI_OPERATION
#define JAVAAPI_OPERATION
#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwiscontext.h"
#include "../../IlwisCore/core/catalog/catalog.h"
#include "../../IlwisCore/core/catalog/catalogview.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operationmetadata.h"
#include "../../IlwisCore/core/ilwisobjects/operation/symboltable.h"
#include "../../IlwisCore/core/ilwisobjects/operation/commandhandler.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operationExpression.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operation.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/catalog/resource.h"

#include "javaapi_object.h"

namespace javaapi {
    class IlwisOperation{
    public:
        IlwisOperation();
        ~IlwisOperation();
        IlwisOperation(Ilwis::Resource& res);
        IlwisOperation(const IlwisOperation& other);
        IlwisOperation& operator=(const IlwisOperation& other);
        std::string getName() const;
        std::string getSyntax() const;
        std::string getLongname() const;
        std::string getKeyword() const;
        std::string getInparameters() const;
        std::string getOutparameters() const;
        qint64 getId() const;
        bool isValid() const;

        std::string getPinName(qint32 orderNumber) const;
        qint64 getPinType(qint32 orderNumber) const;
        std::string getPinDesc(qint32 orderNumber) const;

        std::string getPoutName(qint32 orderNumber) const;
        qint64 getPoutType(qint32 orderNumber) const;
        std::string getPoutDesc(qint32 orderNumber) const;
    private:
        Ilwis::Resource* operation = 0;
    };

}

#endif // JAVAAPI_OPERATION

