#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwiscontext.h"
#include "../../IlwisCore/core/catalog/catalog.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/geometries.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"

#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/ellipsoid.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/geodeticdatum.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/projection.h"
#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/proj4parameters.h"

#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/conventionalcoordinatesystem.h"

#include "javaapi_coordinatesystem.h"
#include "javaapi_error.h"

using namespace javaapi;

CoordinateSystem::CoordinateSystem(Ilwis::ICoordinateSystem *cs): IlwisObject(new Ilwis::IIlwisObject(*cs)){
    delete cs;
}

CoordinateSystem::CoordinateSystem(const std::string& resource){
    QString input (QString::fromStdString(resource));
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
    Ilwis::ICoordinateSystem cs(input, itCOORDSYSTEM);
    if (cs.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(cs));
}

Envelope CoordinateSystem::envelope(){
    return Envelope(this->ptr()->as<Ilwis::CoordinateSystem>()->envelope());
}

void CoordinateSystem::setEnvelope(const Envelope& env){
    Ilwis::Envelope ilwEnv = env.data();
    this->ptr()->as<Ilwis::CoordinateSystem>()->envelope(ilwEnv);
}

bool CoordinateSystem::operator==(const CoordinateSystem &csy){
    return (this->ptr()->ptr()->ilwisType() == itCONVENTIONALCOORDSYSTEM) && this->ptr()->as<Ilwis::ConventionalCoordinateSystem>()->isEqual(csy.ptr()->ptr());
}

bool CoordinateSystem::operator!=(const CoordinateSystem &csy){
    return !this->operator==(csy);
}

std::string CoordinateSystem::toWKT(quint32 spaces) const{
    QString ilwStr = this->ptr()->as<Ilwis::CoordinateSystem>()->toWKT();
    return ilwStr.toStdString();
}

std::string CoordinateSystem::toProj4() const{
    if(const_cast<CoordinateSystem*>(this)->type() == "ConventionalCoordinateSystem"){
        QString ilwStr = this->ptr()->as<Ilwis::ConventionalCoordinateSystem>()->toProj4();
        return ilwStr.toStdString();
    }else{
        throw InvalidObject("No proj4 definition for this kind of coordinatesystem available");
    }
}

Envelope CoordinateSystem::convertEnvelope(const CoordinateSystem& pyCsy, const Envelope& pyEnv){
    Ilwis::ICoordinateSystem ilwCsy = pyCsy.ptr()->as<Ilwis::CoordinateSystem>();
    Ilwis::Envelope ilwEnv = pyEnv.data();
    return Envelope(this->ptr()->as<Ilwis::CoordinateSystem>()->convertEnvelope(ilwCsy, ilwEnv));
}

Coordinate CoordinateSystem::coord2coord(const CoordinateSystem &pyCsy, const Coordinate &pyCoord){
    Ilwis::ICoordinateSystem ilwCsy = pyCsy.ptr()->as<Ilwis::CoordinateSystem>();
    Ilwis::Coordinate ilwCoord = pyCoord.data();
    return Coordinate(this->ptr()->as<Ilwis::CoordinateSystem>()->coord2coord(ilwCsy, ilwCoord));
}

CoordinateSystem *CoordinateSystem::toCoordinateSystem(IObject *obj){
    CoordinateSystem* ptr = dynamic_cast<CoordinateSystem*>(obj);
    if(!ptr)
        throw InvalidObject("cast to CoordinateSystem not possible");
    return ptr;
}
