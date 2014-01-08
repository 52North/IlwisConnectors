#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/angle.h"
#include "../../IlwisCore/core/util/point.h"
#include "../../IlwisCore/core/util/line.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/polygon.h"

#include "pythonapi_util.h"

namespace pythonapi {

    //=============POINT2D=============================

    template<class T> Point2DTemplate<T>::Point2DTemplate(const Ilwis::Point2D<T> &point): _data(new Ilwis::Point2D<T>(point)){
    }

    template<class T> Point2DTemplate<T>::Point2DTemplate(Ilwis::Point<T> *point): _data(point){
    }

    template<class T> Point2DTemplate<T>::Point2DTemplate(T x, T y): _data(new Ilwis::Point2D<T>(x,y)){
    }

    template<class T> Ilwis::Point2D<T>& Point2DTemplate<T>::data() const{
        return (*(Ilwis::Point2D<T>*)this->_data.get());
    }

    template<class T> T Point2DTemplate<T>::x() const{
        return this->data().x();
    }

    template<class T> T Point2DTemplate<T>::y() const{
        return this->data().y();
    }

    template<class T> void Point2DTemplate<T>::setX(T v){
        this->data().x(v);
    }

    template<class T> void Point2DTemplate<T>::setY(T v){
        this->data().y(v);
    }

    //template spezializations
    template<> std::string Point2DTemplate<double>::__str__(){
    return QString("coordinate(%1,%2)").arg(this->data().x(),0,'f',6).arg(this->data().y(),0,'f',6).toStdString();
    }

    template<> std::string Point2DTemplate<qint32>::__str__(){
        return QString("pixel(%1,%2)").arg(this->data().x()).arg(this->data().y()).toStdString();
    }

    template class Point2DTemplate<qint32>;
    template class Point2DTemplate<double>;

    //=============POINT3D=============================

    template<class T> Point3DTemplate<T>::Point3DTemplate(const Ilwis::Point3D<T> &point): Point2DTemplate<T>(new Ilwis::Point3D<T>(point)){
    }

    template<class T> Point3DTemplate<T>::Point3DTemplate(T x, T y, T z): Point2DTemplate<T>(new Ilwis::Point3D<T>(x,y,z)){
    }

    template<class T> Ilwis::Point3D<T>& Point3DTemplate<T>::data() const{
        return (*(Ilwis::Point3D<T>*)this->_data.get());
    }

    template<class T> T Point3DTemplate<T>::z() const{
        return this->data().z();
    }

    template<class T> void Point3DTemplate<T>::setZ(T v){
        this->data().z(v);
    }

    //template spezializations
    template<> std::string Point3DTemplate<double>::__str__(){
        return QString("coordinate(%1,%2,%3)").arg(this->data().x(),0,'f',6).arg(this->data().y(),0,'f',6).arg(this->data().z(),0,'f',6).toStdString();
    }

    template<> std::string Point3DTemplate<qint32>::__str__(){
        //TODO kept (3D)pixel as string representation for use in Engine.do(..) better change to voxel(,,,)
        return QString("pixel(%1,%2,%3)").arg(this->data().x()).arg(this->data().y()).arg(this->data().z()).toStdString();
    }

    template class Point3DTemplate<qint32>;
    template class Point3DTemplate<double>;

    //=============BOX=============================

    template<class T> BoxTemplate<T>::BoxTemplate():_data(new Ilwis::Box3D<T>()){
    }

    template<class T> BoxTemplate<T>::BoxTemplate(const Ilwis::Box3D<T> &box): _data(new Ilwis::Box3D<T>(box)){
    }

    template<class T> BoxTemplate<T>::BoxTemplate(const std::string& envelope): _data(new Ilwis::Box3D<T>(QString::fromStdString(envelope))){
    }

    template<class T> BoxTemplate<T>::BoxTemplate(const Voxel &min, const Voxel &max): _data(new Ilwis::Box3D<T>(min.data(), max.data())){
    }

    template<class T> BoxTemplate<T>::BoxTemplate(const Coordinate &min, const Coordinate &max): _data(new Ilwis::Box3D<T>(min.data(), max.data())){
    }

    template<class T> BoxTemplate<T>::BoxTemplate(const Size &size): _data(new Ilwis::Box3D<T>(size.data())){
    }

    template<class T> std::string BoxTemplate<T>::__str__(){
        return this->data().toString().toStdString();
    }

    template<class T> Size BoxTemplate<T>::size(){
    return Size(this->data().size());
    }

    template<class T> Point3DTemplate<T> BoxTemplate<T>::minCorner(){
        return Point3DTemplate<T>(this->data().min_corner());
    }

    template<class T> Point3DTemplate<T> BoxTemplate<T>::maxCorner(){
        return Point3DTemplate<T>(this->data().max_corner());
    }

    template<class T> bool BoxTemplate<T>::contains(const Point3DTemplate<T> &point){
        return this->data().contains(point.data());
    }

    template<class T> bool BoxTemplate<T>::contains(const BoxTemplate<T>& box){
        return this->data().contains(box.data());
    }

    template<class T> Ilwis::Box3D<T> &BoxTemplate<T>::data() const{
        return (*this->_data);
    }

    template class BoxTemplate<qint32>;
    template class BoxTemplate<double>;

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

    bool Size::__contains__(const Voxel &vox) const{
        return this->data().contains(vox.x(),vox.y(),vox.z());
    }

    std::string Size::__str__(){
        return QString("Size(%1, %2, %3)").arg(this->data().xsize()).arg(this->data().ysize()).arg(this->data().zsize()).toStdString();
    }

    Ilwis::Size &Size::data() const{
        return (*this->_data);
    }

} // namespace pythonapi
