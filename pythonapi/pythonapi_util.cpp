#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/angle.h"
#include "../../IlwisCore/core/util/point.h"
#include "../../IlwisCore/core/util/line.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/polygon.h"

#include "pythonapi_util.h"

namespace pythonapi {
Coordinate::Coordinate(double x, double y): _2d(true), _data(new Ilwis::Point3D<double>(x,y,0)){
}

Coordinate::Coordinate(double x, double y, double z): _2d(false), _data(new Ilwis::Point3D<double>(x,y,z)){
}

std::string Coordinate::__str__(){
    if (this->_2d)
        return QString("coordinate(%1,%2)").arg(this->_data->x()).arg(this->_data->y()).toStdString();
    else
        return QString("coordinate(%1,%2,%3)").arg(this->_data->x()).arg(this->_data->y()).arg(this->_data->z()).toStdString();
}

Ilwis::Point3D<double>& Coordinate::data(){
    return (*this->_data);
}

Pixel::Pixel(qint32 x, qint32 y): _x(x), _y(y){
}

std::string Pixel::__str__(){
    return QString("pixel(%1,%2)").arg(this->_x).arg(this->_y).toStdString();
}

Voxel::Voxel(const Ilwis::Point3D<qint32> &vox): _data(new Ilwis::Point3D<>(vox)){
}

Voxel::Voxel(qint32 x, qint32 y, qint32 z): _data(new Ilwis::Point3D<>(x,y,z)){
}

qint32 Voxel::x() const{
    return this->data().x();
}

qint32 Voxel::y() const{
    return this->data().y();
}

qint32 Voxel::z() const{
    return this->data().z();
}

void Voxel::setX(qint32 v){
    this->data().x(v);
}

void Voxel::setY(qint32 v){
    this->data().y(v);
}

void Voxel::setZ(qint32 v){
    this->data().z(v);
}

std::string Voxel::__str__(){
    //TODO kept (3D)pixel as string representation for use in Engine.do(..) better change to voxel(,,,)
    return QString("pixel(%1,%2,%3)").arg(this->_data->x()).arg(this->_data->y()).arg(this->_data->z()).toStdString();
}

Ilwis::Point3D<qint32> &Voxel::data() const{
    return (*this->_data);
}

Box::Box():_data(new Ilwis::Box3D<>()){
}

Box::Box(const Ilwis::Box3D<qint32>& box): _data(new Ilwis::Box3D<>(box)){
}

Box::Box(const std::string& envelope): _data(new Ilwis::Box3D<>(QString::fromStdString(envelope))){
}

Box::Box(const Voxel &min, const Voxel &max): _data(new Ilwis::Box3D<>(min.data(), max.data())){
}

std::string Box::__str__(){
    return this->data().toString().toStdString();
}

Size Box::size(){
    return Size(this->data().size());
}

Ilwis::Box3D<qint32> &Box::data() const{
    return (*this->_data);
}

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
