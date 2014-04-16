#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericrange.h"
#include "itemrange.h"
#include "interval.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "intervalrange.h"
#include "pythonapi_pyobject.h"
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

//------------------------------------------------------------
void NumericItemRange::add(PyObject *item)
{
    if (CppTupleElementCount(item) == 3 || CppTupleElementCount(item) == 4){
        QString label = QString::fromStdString(CppTupleElement2String(item,0));
        if ( label == sUNDEF)
            return;

        double rmin = CppTupleElement2Double(item, 1);
        double rmax = CppTupleElement2Double(item,2);
        if ( rmin == rUNDEF || rmax == rUNDEF)
            return;
        Ilwis::Interval *numitem;
        if ( CppTupleElementCount(item) == 4){
            double resolution = CppTupleElement2Double(item,3);
            if ( resolution == rUNDEF)
                return;
            numitem = new Ilwis::Interval(label, { rmin, rmax,resolution});
        }else
            numitem = new Ilwis::Interval(label, { rmin, rmax});

        static_cast<Ilwis::ItemRange*>(_range.get())->add(numitem);
    }
}
//------------------------------------------------------------
void NamedItemRange::add(PyObject *item)
{
    QString name = QString::fromStdString(CppString2stdString(item));
    Ilwis::NamedIdentifier *id = new Ilwis::NamedIdentifier(name);
    static_cast<Ilwis::ItemRange*>(_range.get())->add(id);

}
//-----------------------------------------------------------
void ThematicRange::add(PyObject *item)
{
    int elements = CppTupleElementCount(item);

    if ( elements > 0 && elements <=3 ) {
        QString description = sUNDEF, code = sUNDEF;
        QString label = QString::fromStdString(CppTupleElement2String(item,0));
        if ( label == sUNDEF)
            return;
        if ( elements == 2){
            description = QString::fromStdString(CppTupleElement2String(item,1));
        }
        if ( elements == 2){
            code = QString::fromStdString(CppTupleElement2String(item,2));
        }
        Ilwis::ThematicItem *titem = new Ilwis::ThematicItem({label, code, description});
        static_cast<Ilwis::ItemRange*>(_range.get())->add(titem);
    }
}
