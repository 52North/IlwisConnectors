#include "../../IlwisCore/core/identity.h"
#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwiscontext.h"
#include "../../IlwisCore/core/catalog/catalog.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/catalog/resource.h"
#include "../../IlwisCore/core/catalog/catalogview.h"

#include "javaapi_catalog.h"
#include "javaapi_error.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericdomain.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributedefinition.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/factory.h"
#include "../../IlwisCore/core/abstractfactory.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/featurefactory.h"

#include "javaapi_featurecoverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/raster.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/rastercoverage.h"
#include "javaapi_rastercoverage.h"

#include "javaapi_domain.h"

#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/projection.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/ellipsoid.h"
#include "../../IlwisCore/core/ilwisobjects/operation/operationmetadata.h"

namespace javaapi {

    Catalog::Catalog(const std::string& url, const std::string& filter){
        QString input (QString::fromStdString(url));
        input.replace('\\','/');
        // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
        if (input.indexOf("://") < 0) {
            int pos = input.indexOf('/');
            if (pos > 0) {
                if ((input.count('/') > 1) || QFileInfo(input).exists()) // full path starting with drive-letter (MS-DOS-style)
                    input = "file:///" + input;
                else // container object without path, e.g. myfile.hdf/subdataset: look for it in workingCatalog()
                    input = "file:///" + Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
            }  else if (pos == 0) // full path starting with path-separator (UNIX-style)
                input = "file://" + input;
            else { // pos < 0: file without path, or new object
                QString file = Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
                if (QFileInfo (file).exists()) // file without path
                    input = "file:///" + file;
            }
        }
        QUrl location (input);
        Ilwis::CatalogView* cat = new Ilwis::CatalogView(location);
        cat->filter(QString::fromStdString(filter));
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

    std::vector<std::string> Catalog::items(){
        if (this->__bool__()){
            std::vector<Ilwis::Resource> itms = this->_data->items();
            std::vector<std::string> ret;
            int i = 0;
            for(auto it = itms.begin(); it < itms.end(); it++ ){
                /*if (!setTupleItem(ret, i, PyUnicodeFromString(it->name().toStdString().data()))){
                    throw InvalidObject("invalid recource in catalog items");
                }*/
                ret.push_back( it->name().toStdString() );
                i++;
            }
            return ret;
        }else{
            return std::vector<std::string>();
        }
    }

    IObject* Catalog::_getitem(const std::string &name){
        Ilwis::Resource res;
        std::vector<Ilwis::Resource> itms = this->_data->items();
        for(auto it = itms.begin(); it < itms.end(); it++ ){
            if(it->name().compare(QString::fromStdString(name), Qt::CaseInsensitive) == 0){
                IlwisTypes type = it->ilwisType();
                if (hasType(type,itFEATURE)){
                    return new FeatureCoverage(new Ilwis::IFeatureCoverage(*it));
                }else if (hasType(type,itRASTER)){
                    return new RasterCoverage(new Ilwis::IRasterCoverage(*it));
                }else if (hasType(type,itTABLE)){
                    return new Table(new Ilwis::ITable(*it));
                }else if (hasType(type,itNUMERICDOMAIN)){
                    return new NumericDomain(new Ilwis::INumericDomain(*it));
                }else if (hasType(type,itCOORDSYSTEM)){
                    return new CoordinateSystem(new Ilwis::ICoordinateSystem(*it));
                }else if (hasType(type,itGEOREF)){
                    return new GeoReference(new Ilwis::IGeoReference(*it));
//                }else if (hasType(type,itOPERATIONMETADATA)){
//                    return new OperationMetaData(new Ilwis::IOperationMetaData(*it));
//                }else if (hasType(type,itPROJECTION)){
//                    return new Projection(new Ilwis::IProjection(*it));
//                }else if (hasType(type,itELLIPSOID)){
//                    return new Ellipsoid(new Ilwis::IEllipsoid(*it));
                }else if (hasType(type,itCATALOG)){
                    Ilwis::CatalogView* cat = new Ilwis::CatalogView(it->url());
                    cat->prepare();
                    return new Catalog(cat);
                }else{
                    return new IlwisObject(new Ilwis::IIlwisObject(*it));
                }
            }
        }
        throw std::out_of_range(std::string("item not found: ")+name);
    }

    Catalog *Catalog::toCatalog(IObject *obj){
        Catalog* ptr = dynamic_cast<Catalog*>(obj);
        if(!ptr)
            throw InvalidObject("cast to Catalog not possible");
        return ptr;
    }

    Catalog::Catalog(Ilwis::CatalogView *cat){
        this->_data.reset(cat);
    }

} // namespace javaapi
