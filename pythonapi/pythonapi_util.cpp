#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/angle.h"
#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Coordinate.h"

#include "../../IlwisCore/core/util/location.h"
#include "../../IlwisCore/core/util/coordinate.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/location.h"

#include "pythonapi_util.h"

namespace pythonapi {

    //=============Coordinate=============================

    Coordinate::Coordinate(const Ilwis::Coordinate& coordinate): _data(new Ilwis::Coordinate(coordinate)){
    }

    Coordinate::Coordinate(Ilwis::Coordinate* coordinate): _data(coordinate){
    }

    Coordinate::Coordinate(double x, double y): _data(new Ilwis::Coordinate(x,y)){
    }

    Coordinate::Coordinate(double x, double y, double z): _data(new Ilwis::Coordinate(x,y,z)){
    }

    Ilwis::Coordinate& Coordinate::data() const{
        return *this->_data;
    }

    double Coordinate::x() const{
        return this->data().x;
    }

    double Coordinate::y() const{
        return this->data().y;
    }

    double Coordinate::z() const{
        return this->data().z;
    }

    void Coordinate::setX(double v){
        this->data().x = v;
    }

    void Coordinate::setY(double v){
        this->data().y = v;
    }

    void Coordinate::setZ(double v){
        this->data().z = v;
    }

    std::string Coordinate::__str__(){
        if (this->data().z == Ilwis::rUNDEF)
            return QString("coordinate(%1,%2)").arg(this->data().x,0,'f',6).arg(this->data().y,0,'f',6).toStdString();
        else
            return QString("coordinate(%1,%2,%3)").arg(this->data().x,0,'f',6).arg(this->data().y,0,'f',6).arg(this->data().z,0,'f',6).toStdString();
    }

    //=============Pixel=============================

    template<class CrdType> PixelTemplate<CrdType>::PixelTemplate(const Ilwis::Location<CrdType, false>& pixel):
        _data(new Ilwis::Location<CrdType, false>(pixel)){
    }

    template<class CrdType> PixelTemplate<CrdType>::PixelTemplate(const PixelTemplate<qint32>& pixel):
        _data(new Ilwis::Location<CrdType, false>(pixel.data())){
    }

    template<class CrdType> PixelTemplate<CrdType>::PixelTemplate(const PixelTemplate<double>& pixel):
        _data(new Ilwis::Location<CrdType, false>(pixel.data())){
    }

    template<class CrdType> PixelTemplate<CrdType>::PixelTemplate(Ilwis::Location<CrdType, false>* pixel):
        _data(pixel){
    }

    template<class CrdType> PixelTemplate<CrdType>::PixelTemplate(CrdType x, CrdType y):
        _data(new Ilwis::Location<CrdType, false>(x,y)){
    }

    template<class CrdType> PixelTemplate<CrdType>::PixelTemplate(CrdType x, CrdType y, CrdType z):
        _data(new Ilwis::Location<CrdType, false>(x,y,z)){
    }

    template<class CrdType> Ilwis::Location<CrdType, false>& PixelTemplate<CrdType>::data() const{
        return *this->_data;
    }

    template<class CrdType> CrdType PixelTemplate<CrdType>::x() const{
        return this->data().x;
    }

    template<class CrdType> CrdType PixelTemplate<CrdType>::y() const{
        return this->data().y;
    }

    template<class CrdType> CrdType PixelTemplate<CrdType>::z() const{
        return this->data().z;
    }

    template<class CrdType> void PixelTemplate<CrdType>::setX(CrdType v){
        this->data().x = v;
    }

    template<class CrdType> void PixelTemplate<CrdType>::setY(CrdType v){
        this->data().y = v;
    }

    template<class CrdType> void PixelTemplate<CrdType>::setZ(CrdType v){
        this->data().z = v;
    }

    template<class CrdType> bool PixelTemplate<CrdType>::is3D() const{
        return this->data().is3D();
    }

    template<class CrdType> std::string PixelTemplate<CrdType>::__str__(){
        if (this->data().is3D())
            return QString("pixel(%1,%2,%3)").arg(this->data().x).arg(this->data().y).arg(this->data().z).toStdString();
        else
            return QString("pixel(%1,%2)").arg(this->data().x).arg(this->data().y).toStdString();
    }

    template class PixelTemplate<qint32>;
    template class PixelTemplate<double>;
    //=============BOX=============================

    template<class IlwisType, class PyType> BoxTemplate<IlwisType, PyType>::BoxTemplate():
        _data(new Ilwis::Box<IlwisType>()){
    }

    template<class IlwisType, class PyType> BoxTemplate<IlwisType, PyType>::BoxTemplate(const Ilwis::Box<IlwisType> &box):
        _data(new Ilwis::Box<IlwisType>(box)){
    }

    template<class IlwisType, class PyType> BoxTemplate<IlwisType, PyType>::BoxTemplate(const std::string& envelope):
        _data(new Ilwis::Box<IlwisType>(QString::fromStdString(envelope))){
    }

    template<class IlwisType, class PyType> BoxTemplate<IlwisType, PyType>::BoxTemplate(const PyType &min, const PyType &max):
        _data(new Ilwis::Box<IlwisType>(min.data(), max.data())){
    }

    template<class IlwisType, class PyType> BoxTemplate<IlwisType, PyType>::BoxTemplate(const Size &size):
        _data(new Ilwis::Box<IlwisType>(size.data())){
    }

    template<class IlwisType, class PyType> BoxTemplate<IlwisType, PyType>::BoxTemplate(const geos::geom::Envelope *envelope){
        _data.reset(new Ilwis::Box<IlwisType>(IlwisType(envelope->getMinX(),envelope->getMinY()),IlwisType(envelope->getMaxX(),envelope->getMaxY())));
    }

    template<class IlwisType, class PyType> std::string BoxTemplate<IlwisType, PyType>::__str__(){
        return this->data().toString().toStdString();
    }

    template<class IlwisType, class PyType> Size BoxTemplate<IlwisType, PyType>::size(){
    return Size(this->data().size());
    }

    template<class IlwisType, class PyType> PyType BoxTemplate<IlwisType, PyType>::minCorner(){
        return PyType(this->data().min_corner());
    }

    template<class IlwisType, class PyType> PyType BoxTemplate<IlwisType, PyType>::maxCorner(){
        return PyType(this->data().max_corner());
    }

    template<class IlwisType, class PyType> bool BoxTemplate<IlwisType, PyType>::contains(const PyType &point){
        return this->data().contains(point.data());
    }

    template<class IlwisType, class PyType> bool BoxTemplate<IlwisType, PyType>::contains(const BoxTemplate<IlwisType, PyType>& box){
        return this->data().contains(box.data());
    }

    template<class IlwisType, class PyType> Ilwis::Box<IlwisType>& BoxTemplate<IlwisType, PyType>::data() const{
        return (*this->_data);
    }

    template class BoxTemplate<Ilwis::Coordinate, Coordinate>;
    template class BoxTemplate<Ilwis::Location<qint32, false>, Pixel>;

    //=============SIZE=============================

    Size::Size(qint32 xsize, qint32 ysize, qint32 zsize):_data(new Ilwis::Size(xsize,ysize,zsize)){
    }

    Size::Size(const Ilwis::Size &size):_data(new Ilwis::Size(size)){
    }

    Size* Size::operator+=(const Size &sz){
        return new Size(this->data() += sz.data());
    }

    Size* Size::operator-=(const Size &sz){
        return new Size(this->data() -= sz.data());
    }

    Size* Size::operator*=(double f){
        return new Size(this->data() *= f);
    }

    bool Size::operator==(const Size &sz){
        return xsize() == sz.xsize() && ysize() == sz.ysize() && zsize() == sz.zsize();
    }

    qint32 Size::xsize() const{
        return this->data().xsize();
    }

    qint32 Size::ysize() const{
        return this->data().ysize();
    }

    qint32 Size::zsize() const{
        return this->data().zsize();
    }

    void Size::setXsize(qint32 x){
        this->data().xsize(x);
    }

    void Size::setYsize(qint32 y){
        this->data().ysize(y);
    }

    void Size::setZsize(qint32 z){
        this->data().zsize(z);
    }

    quint64 Size::linearSize() const{
        return this->data().totalSize();
    }

    bool Size::__contains__(const Pixel& pix) const{
        return this->data().contains(pix.x(),pix.y(),pix.z());
    }

    std::string Size::__str__(){
        return QString("Size(%1, %2, %3)").arg(this->data().xsize()).arg(this->data().ysize()).arg(this->data().zsize()).toStdString();
    }

    Ilwis::Size &Size::data() const{
        return (*this->_data);
    }

} // namespace pythonapi
