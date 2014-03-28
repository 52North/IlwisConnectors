#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "numericrange.h"
#include "itemrange.h"

#include "pythonapi_range.h"
#include "pythonapi_qvariant.h"

using namespace pythonapi;

Range::Range() {

}

Range::Range(Ilwis::Range *rng) : _range(rng) {

}

bool Range::__bool__() const
{
    return _range->isValid();
}

std::string Range::__str__() 
{
    return _range->toString().toStdString();
}

IlwisTypes Range::valueType() 
{
    return _range->valueType();
}

PyObject *Range::ensure(const PyObject *value, bool inclusive) const
{
   std::unique_ptr<QVariant> v(PyObject2QVariant(value));
   QVariant var = _range->ensure(*v, inclusive);
   return QVariant2PyObject(var);
}

bool Range::isContinuous() const
{
    return _range->isContinuous();
}

PyObject *Range::impliedValue(const PyObject *value) const {
    std::unique_ptr<QVariant> v(PyObject2QVariant(value));
    QVariant var = _range->impliedValue(*v);
    return QVariant2PyObject(var);
}

bool Range::contains(const PyObject *value, bool inclusive) const
{
   std::unique_ptr<QVariant> v(PyObject2QVariant(value));
   return _range->contains(*v, inclusive);
}

//----------------------------------------------------------------------------
NumericRange::NumericRange(double mi, double ma, double resolution)
{
    _range.reset(new Ilwis::NumericRange(mi, ma,resolution))    ;
}

NumericRange::NumericRange(const NumericRange &vr) : NumericRange(vr.min(), vr.max(), vr.resolution())
{
}

bool NumericRange::contains(double v, bool inclusive) const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->contains(v, inclusive);
}

double NumericRange::max() const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->max();
}

double NumericRange::min() const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->max();
}

void NumericRange::setMax(double value)
{
    static_cast<Ilwis::NumericRange*>(_range.get())->max(value);
}

void NumericRange::setMin(double value)
{
    static_cast<Ilwis::NumericRange*>(_range.get())->min(value);
}

double NumericRange::distance() const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->distance();
}

void NumericRange::setResolution(double resolution)
{
    static_cast<Ilwis::NumericRange*>(_range.get())->resolution(resolution);
}

double NumericRange::resolution() const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->resolution();
}

void NumericRange::set(const NumericRange &vr)
{
    setResolution(vr.resolution());
    setMax(vr.max());
    setMin(vr.min());
}

void NumericRange::clear()
{
    static_cast<Ilwis::NumericRange*>(_range.get())->clear();
}

//-------------------------------------------------------------
void ItemRange::remove(const std::string &name)
{
    static_cast<Ilwis::ItemRange*>(_range.get())->remove(name.c_str());
}

void ItemRange::clear()
{
    static_cast<Ilwis::ItemRange*>(_range.get())->clear();
}
