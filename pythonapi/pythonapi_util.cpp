#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/angle.h"
#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/Envelope.inl"

#include "../../IlwisCore/core/util/location.h"
#include "../../IlwisCore/core/util/coordinate.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/location.h"

#include "pythonapi_util.h"

namespace pythonapi {

    //=============Pixel=============================

    template<class CrdType> PixelTemplate<CrdType>::PixelTemplate(const Ilwis::Location<CrdType, false>& pixel):
        _data(new Ilwis::Location<CrdType, false>(pixel)){
    }

    template<class CrdType> PixelTemplate<CrdType>::PixelTemplate(const PixelTemplate<qint32>& pixel):
        _data(pixel.is3D() ? new Ilwis::Location<CrdType, false>(pixel.x(), pixel.y(), pixel.z()) :  new Ilwis::Location<CrdType, false>(pixel.x(), pixel.y())){
    }

    template<class CrdType> PixelTemplate<CrdType>::PixelTemplate(const PixelTemplate<double>& pixel):
        _data(pixel.is3D() ? new Ilwis::Location<CrdType, false>(pixel.x(), pixel.y(), pixel.z()) :  new Ilwis::Location<CrdType, false>(pixel.x(), pixel.y())){
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

    template<class CrdType> PixelTemplate<CrdType>* PixelTemplate<CrdType>::operator *=(CrdType n){
        return new PixelTemplate<CrdType>(this->data() *= n);
    }

    template<class CrdType> PixelTemplate<CrdType>* PixelTemplate<CrdType>::__itruediv__(CrdType n){
    return new PixelTemplate<CrdType>(this->data() /= n);
    }

    template<class CrdType> bool PixelTemplate<CrdType>::operator==(const PixelTemplate<CrdType> &other){
        return this->data() == other.data();
    }

    template<class CrdType> bool PixelTemplate<CrdType>::operator!=(const PixelTemplate<CrdType> &other){
        return this->data() != other.data();
    }

    template<class CrdType> std::string PixelTemplate<CrdType>::__str__(){
        if (this->data().is3D())
            return QString("pixel(%1,%2,%3)").arg(this->data().x).arg(this->data().y).arg(this->data().z).toStdString();
        else
            return QString("pixel(%1,%2)").arg(this->data().x).arg(this->data().y).toStdString();
    }

    template<class CrdType> bool PixelTemplate<CrdType>::__bool__() const{
        return this->data().isValid();
    }

    template class PixelTemplate<qint32>;
    template class PixelTemplate<double>;

    //=============Coordinate=============================

    Coordinate::Coordinate(const Ilwis::Coordinate& coordinate):
        _data(new Ilwis::Coordinate(coordinate)){
    }

    Coordinate::Coordinate(const Coordinate& crd):
        _data(crd.is3D() ? new Ilwis::Coordinate(crd.x(), crd.y(), crd.z()) : new Ilwis::Coordinate(crd.x(), crd.y())){
    }

    Coordinate::Coordinate(Ilwis::Coordinate* coordinate):
        _data(coordinate){
    }

    Coordinate::Coordinate(double x, double y):
        _data(new Ilwis::Coordinate(x,y)){
    }

    Coordinate::Coordinate(double x, double y, double z):
        _data(new Ilwis::Coordinate(x,y,z)){
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

    bool Coordinate::is3D() const{
        return this->data().is3D();
    }

    Coordinate *Coordinate::operator*=(double n){
        return new Coordinate(this->data() *= n);
    }

    Coordinate *Coordinate::__itruediv__(double n){
        return new Coordinate(this->data() /= n);
    }

    bool Coordinate::operator==(const Coordinate &other){
        return this->data() == other.data();
    }

    bool Coordinate::operator!=(const Coordinate &other){
        return this->data() != other.data();
    }

    std::string Coordinate::__str__(){
        if (this->data().z == Ilwis::rUNDEF)
            return QString("coordinate(%1,%2)").arg(this->data().x,0,'f',6).arg(this->data().y,0,'f',6).toStdString();
        else
            return QString("coordinate(%1,%2,%3)").arg(this->data().x,0,'f',6).arg(this->data().y,0,'f',6).arg(this->data().z,0,'f',6).toStdString();
    }

    bool Coordinate::__bool__() const{
        return this->data().isValid();
    }

    //=========================SIZE=============================

    template<typename T> SizeTemplate<T>::SizeTemplate(T xsize, T ysize, T zsize):
        _data(new Ilwis::Size<T>(xsize,ysize,zsize)){
    }

    template<typename T> SizeTemplate<T>::SizeTemplate(const Ilwis::Size<T>& size):
        _data(new Ilwis::Size<T>(size)){
    }

    template<typename T> SizeTemplate<T>::SizeTemplate(const SizeTemplate<quint32> &size):
        _data(new Ilwis::Size<T>(size.xsize(),size.ysize(),size.zsize())){
    }

    template<typename T> SizeTemplate<T>::SizeTemplate(const SizeTemplate<double> &size):
        _data(new Ilwis::Size<T>(size.xsize(),size.ysize(),size.zsize())){
    }

    template<typename T> SizeTemplate<T>* SizeTemplate<T>::operator+=(const SizeTemplate<T>& sz){
        return new SizeTemplate(this->data() += sz.data());
    }

    template<typename T> SizeTemplate<T>* SizeTemplate<T>::operator-=(const SizeTemplate<T>& sz){
        return new SizeTemplate(this->data() -= sz.data());
    }

    template<typename T> SizeTemplate<T>* SizeTemplate<T>::operator*=(double f){
        return new SizeTemplate<T>(this->data() *= f);
    }

    template<typename T> bool SizeTemplate<T>::operator==(const SizeTemplate<T>& sz){
        return (xsize() == sz.xsize()) && (ysize() == sz.ysize()) && (zsize() == sz.zsize());
    }

    template<typename T> bool SizeTemplate<T>::operator!=(const SizeTemplate<T>& sz){
        return !((*this) == sz);
    }

    template<typename T> T SizeTemplate<T>::xsize() const{
        return this->data().xsize();
    }

    template<typename T> T SizeTemplate<T>::ysize() const{
        return this->data().ysize();
    }

    template<typename T> T SizeTemplate<T>::zsize() const{
        return this->data().zsize();
    }

    template<typename T> void SizeTemplate<T>::setXsize(T x){
        this->data().xsize(x);
    }

    template<typename T> void SizeTemplate<T>::setYsize(T y){
        this->data().ysize(y);
    }

    template<typename T> void SizeTemplate<T>::setZsize(T z){
        this->data().zsize(z);
    }

    template<typename T> quint64 SizeTemplate<T>::linearSize() const{
        return this->data().linearSize();
    }

    template<typename T> bool SizeTemplate<T>::__contains__(const Coordinate& pix) const{
        return this->data().contains(pix.x(),pix.y(),pix.z());
    }

    template<typename T> bool SizeTemplate<T>::__contains__(const PixelTemplate<qint32>& pix) const{
        return this->data().contains(pix.x(),pix.y(),pix.z());
    }

    template<typename T> bool SizeTemplate<T>::__contains__(const PixelTemplate<double>& pix) const{
        return this->data().contains(pix.x(),pix.y(),pix.z());
    }

    template<typename T> std::string SizeTemplate<T>::__str__(){
        return QString("Size(%1, %2, %3)").arg(this->data().xsize()).arg(this->data().ysize()).arg(this->data().zsize()).toStdString();
    }

    template<typename T> bool SizeTemplate<T>::__bool__() const{
        return this->data().isValid();
    }

    template<typename T> Ilwis::Size<T>& SizeTemplate<T>::data() const{
        return (*this->_data);
    }

    template class SizeTemplate<quint32>;
    template class SizeTemplate<double>;

    //=============BOX=============================

    template<class IlwisType, class PyType, typename SizeType>
    BoxTemplate<IlwisType, PyType, SizeType>::BoxTemplate():
        _data(new Ilwis::Box<IlwisType>()){
    }

    template<class IlwisType, class PyType, typename SizeType>
    BoxTemplate<IlwisType, PyType, SizeType>::BoxTemplate(const Ilwis::Box<IlwisType> &box):
        _data(new Ilwis::Box<IlwisType>(box)){
    }

    template<class IlwisType, class PyType, typename SizeType>
    BoxTemplate<IlwisType, PyType, SizeType>::BoxTemplate(const std::string& envelope):
        _data(new Ilwis::Box<IlwisType>(QString::fromStdString(envelope))){
    }

    template<class IlwisType, class PyType, typename SizeType>
    BoxTemplate<IlwisType, PyType, SizeType>::BoxTemplate(const PyType &min, const PyType &max):
        _data(new Ilwis::Box<IlwisType>(min.data(), max.data())){
    }

    template<class IlwisType, class PyType, typename SizeType>
    BoxTemplate<IlwisType, PyType, SizeType>::BoxTemplate(const SizeTemplate<SizeType>& size):
        _data(new Ilwis::Box<IlwisType>(size.data())){
    }

    template<class IlwisType, class PyType, typename SizeType>
    BoxTemplate<IlwisType, PyType, SizeType>::BoxTemplate(const geos::geom::Envelope *envelope){
        _data.reset(new Ilwis::Box<IlwisType>(
                        IlwisType(envelope->getMinX(),envelope->getMinY()),
                        IlwisType(envelope->getMaxX(),envelope->getMaxY())
                    ));
    }

    template<class IlwisType, class PyType, typename SizeType>
    std::string BoxTemplate<IlwisType, PyType, SizeType>::__str__(){
        return this->data().toString().toStdString();
    }

    template<class IlwisType, class PyType, typename SizeType>
    bool BoxTemplate<IlwisType, PyType, SizeType>::__bool__() const{
        return this->data().isValid();
    }

    template<class IlwisType, class PyType, typename SizeType>
    SizeTemplate<SizeType> BoxTemplate<IlwisType, PyType, SizeType>::size(){
        return SizeTemplate<SizeType>(this->data().template size<SizeType>());
    }

    template<class IlwisType, class PyType, typename SizeType>
    bool BoxTemplate<IlwisType, PyType, SizeType>::is3D() const{
        return this->data().is3D();
    }

    template<class IlwisType, class PyType, typename SizeType>
    PyType BoxTemplate<IlwisType, PyType, SizeType>::minCorner(){
        return PyType(this->data().min_corner());
    }

    template<class IlwisType, class PyType, typename SizeType>
    PyType BoxTemplate<IlwisType, PyType, SizeType>::maxCorner(){
        return PyType(this->data().max_corner());
    }

    template<class IlwisType, class PyType, typename SizeType>
    bool BoxTemplate<IlwisType, PyType, SizeType>::__contains__(const PyType &point) const{
        return this->data().contains(point.data());
    }

    template<class IlwisType, class PyType, typename SizeType>
    bool BoxTemplate<IlwisType, PyType, SizeType>::__contains__(const BoxTemplate<IlwisType, PyType, SizeType>& box) const{
        return this->data().contains(box.data());
    }

    template<class IlwisType, class PyType, typename SizeType>
    bool BoxTemplate<IlwisType, PyType, SizeType>::operator==(const BoxTemplate<IlwisType, PyType, SizeType> &other){
        return (this->data() == other.data());
    }

    template<class IlwisType, class PyType, typename SizeType>
    bool BoxTemplate<IlwisType, PyType, SizeType>::operator!=(const BoxTemplate<IlwisType, PyType, SizeType> &other){
        return (this->data() != other.data());
    }

    template<class IlwisType, class PyType, typename SizeType>
    Ilwis::Box<IlwisType>& BoxTemplate<IlwisType, PyType, SizeType>::data() const{
        return (*this->_data);
    }

    template class BoxTemplate<Ilwis::Coordinate, Coordinate, double>;
    template class BoxTemplate<Ilwis::Location<qint32, false>, Pixel, quint32>;

} // namespace pythonapi
