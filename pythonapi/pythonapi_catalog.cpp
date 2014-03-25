#include "../../IlwisCore/core/identity.h"
#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/catalog/resource.h"
#include "../../IlwisCore/core/catalog/catalogview.h"

#include "pythonapi_catalog.h"

#include "pythonapi_pyobject.h"
#include "pythonapi_error.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericdomain.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/factory.h"
#include "../../IlwisCore/core/abstractfactory.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featurefactory.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"
#include "pythonapi_featurecoverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/raster.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/rastercoverage.h"
#include "pythonapi_rastercoverage.h"

namespace pythonapi {

    Catalog::Catalog(const std::string& url, const std::string& filter){
        Ilwis::CatalogView* cat = new Ilwis::CatalogView();
        cat->prepare(QUrl(QString::fromStdString(url)), QString::fromStdString(filter));
        this->_data.reset(cat);
    }

    Catalog::~Catalog(){
    }

    bool Catalog::__bool__() const{
        return this->_data->isValid();
    }

    std::string Catalog::__str__(){
        if (this->__bool__())
            return "Catalog";
        else
            return "invalid Catalog";
    }

    IlwisTypes Catalog::ilwisType(){
        return itCATALOG;
    }

    PyObject* Catalog::items(){
        if (this->__bool__()){
            std::vector<Ilwis::Resource> itms = this->_data->items();
            PyObject* ret = newPyTuple(itms.size());
            int i = 0;
            for(auto it = itms.begin(); it < itms.end(); it++ ){
                if (!setTupleItem(ret, i, PyUnicodeFromString(it->name().toStdString().data()))){
                    throw InvalidObject("invalid recource in catalog items");
                }
                i++;
            }
            return ret;
        }else{
            return newPyTuple(0);
        }
    }

    IlwisObject* Catalog::_getitem(const std::string &name){
        Ilwis::Resource res;
        std::vector<Ilwis::Resource> itms = this->_data->items();
        for(auto it = itms.begin(); it < itms.end(); it++ ){
            if(it->name().compare(QString::fromStdString(name), Qt::CaseInsensitive) == 0){
                IlwisTypes type = it->ilwisType();
                if (hasType(type,itFEATURE)){//TODO extent for itCOORDSYSTEM,itGEOREF, etc..
                    return new FeatureCoverage(new Ilwis::IFeatureCoverage(*it));
                }else if (hasType(type,itRASTER)){
                    return new RasterCoverage(new Ilwis::IRasterCoverage(*it));
                }else{
                    return new IlwisObject(new Ilwis::IIlwisObject(*it));
                }
            }
        }
        throw std::out_of_range(std::string("item not found: ")+name);
    }



} // namespace pythonapi
