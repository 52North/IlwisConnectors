#ifndef JAVAAPI_ENGINE_H
#define JAVAAPI_ENGINE_H

#include "javaapi_object.h"
#include "javaapi_ilwisoperation.h"

namespace javaapi {
    class Catalog;
    class Engine{
    public:
        static IObject* _do(std::string output_name, std::string operation,std::string c3 = "",std::string c4 = "",std::string c5 = "",std::string c6 = "",std::string c7="", std::string c8="", std::string c9="");
        static void setWorkingCatalog(const std::string& location);
        static std::string getLocation();
        static std::vector<std::string> operations(const std::string& filter = "*");
        static std::string operationMetaData(const std::string& name);
        //static PyObject* _catalogItems();
        static std::vector<IlwisOperation> operationVector(const std::string& name);
        static std::vector<IlwisOperation> getAllOperations();
        static IlwisOperation getOperationById(qint64 id);
    private:
        Engine();
    };

}
#endif // JAVAAPI_ENGINE_H
