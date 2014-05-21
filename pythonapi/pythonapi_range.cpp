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
#include "identifierrange.h"
#include "colorrange.h"
#include "coloritem.h"
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

IlwisTypes Range::ilwisType()
{
    return itANY;
}

IlwisTypes Range::valueType() const
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

void ItemRange::count()
{
    static_cast<Ilwis::ItemRange*>(_range.get())->count();
}

void ItemRange::clear()
{
    static_cast<Ilwis::ItemRange*>(_range.get())->clear();
}

//------------------------------------------------------------

NumericItemRange::NumericItemRange()
{
    _range.reset(new Ilwis::IntervalRange());
}

NumericItemRange::NumericItemRange(PyObject *item)
{
    _range.reset(new Ilwis::IntervalRange());
    add(item);
}

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
NamedItemRange::NamedItemRange()
{
    _range.reset(new Ilwis::NamedIdentifierRange());
}

NamedItemRange::NamedItemRange(PyObject *item)
{
    _range.reset(new Ilwis::NamedIdentifierRange());
    add(item);
}

void NamedItemRange::add(PyObject *item)
{
    QString name = QString::fromStdString(CppString2stdString(item));
    Ilwis::NamedIdentifier *id = new Ilwis::NamedIdentifier(name);
    static_cast<Ilwis::ItemRange*>(_range.get())->add(id);

}
//-----------------------------------------------------------
ThematicRange::ThematicRange()
{
    _range.reset(new Ilwis::ThematicRange());
}

ThematicRange::ThematicRange(PyObject *item)
{
    _range.reset(new Ilwis::ThematicRange());
    add(item);
}

void ThematicRange::add(PyObject *item)
{
    int elements = CppTupleElementCount(item);

    if ( elements > 0 && elements <=3 ) {
        QString description = sUNDEF, code = sUNDEF;
        QString label = QString::fromStdString(CppTupleElement2String(item,0));
        if ( label == sUNDEF)
            return;
        if ( elements >= 2){
            code = QString::fromStdString(CppTupleElement2String(item,1));
        }
        if ( elements == 3){
            description = QString::fromStdString(CppTupleElement2String(item,2));
        }
        Ilwis::ThematicItem *titem = new Ilwis::ThematicItem({label, code, description});
        static_cast<Ilwis::ItemRange*>(_range.get())->add(titem);
    }
}

//----------------------------------------------------------------

Color::Color(){
    _colorVal = PyDictNew();
}

Color::Color(ColorModel type, PyObject* obj){
    _colorVal = PyDictNew();
    readColor(type, obj);
}

void Color::readColor(ColorModel type, PyObject* obj)
{
    PyObject* dict = PyDictNew();
    _type = type;
    if(PyDictCheckExact(obj))
    {
        _colorVal = obj;
        return;
    }else if(PyTupleCheckExact(obj)){
        if(CppTupleElementCount(obj) >= 4){
            switch(type){
            case ColorModel::cmCYMKA:
                PyDictSetItemString(dict, "cyan", PyTupleGetItem(obj, 0));
                PyDictSetItemString(dict, "magenta", PyTupleGetItem(obj, 1));
                PyDictSetItemString(dict, "yellow", PyTupleGetItem(obj, 2));
                PyDictSetItemString(dict, "black", PyTupleGetItem(obj, 3));
                if(CppTupleElementCount(obj) == 5)
                    PyDictSetItemString(dict, "alpha", PyTupleGetItem(obj, 4));
                break;
            case ColorModel::cmHSLA:
                PyDictSetItemString(dict, "hue", PyTupleGetItem(obj, 0));
                PyDictSetItemString(dict, "lightness", PyTupleGetItem(obj, 1));
                PyDictSetItemString(dict, "saturation", PyTupleGetItem(obj, 2));
                PyDictSetItemString(dict, "alpha", PyTupleGetItem(obj, 3));
                break;
            case ColorModel::cmGREYSCALE:
            case ColorModel::cmRGBA:
                PyDictSetItemString(dict, "red", PyTupleGetItem(obj, 0));
                PyDictSetItemString(dict, "green", PyTupleGetItem(obj, 1));
                PyDictSetItemString(dict, "blue", PyTupleGetItem(obj, 2));
                PyDictSetItemString(dict, "alpha", PyTupleGetItem(obj, 3));
                break;
            }
        }
    }

    _colorVal = dict;
}

double Color::getItem(std::string str) const{
    const char* key = str.c_str();
    return CppFloat2Double(PyDictGetItemString(_colorVal,key));
}


ColorModel Color::getColorModel() const{
    return _type;
}

//-----------------------------------------------------------------

ColorRange::ColorRange(){

}

ColorRange::ColorRange(IlwisTypes tp, ColorModel clrmodel)
{
    int enumVal = clrmodel;
    Ilwis::ColorRange::ColorModel ilwCol = static_cast<Ilwis::ColorRange::ColorModel>(enumVal);
    //_range.reset(new Ilwis::ColorRange(tp, ilwCol));
}

ColorModel ColorRange::defaultColorModel() const
{
    int enumVal = static_cast<const Ilwis::ColorRange*>(_range.get())->defaultColorModel();
    ColorModel colormodel = static_cast<ColorModel>(enumVal);
    return colormodel;

}

void ColorRange::defaultColorModel(ColorModel m)
{
    int enumVal = m;
    Ilwis::ColorRange::ColorModel ilwCol = static_cast<Ilwis::ColorRange::ColorModel>(enumVal);
    static_cast<Ilwis::ColorRange*>(_range.get())->defaultColorModel(ilwCol);
}

Color ColorRange::toColor(PyObject *v, ColorModel colortype)
{
    if ( PyTupleCheckExact(v))
        return Color(colortype, v);
    else if ( PyUnicodeCheckExact(v)){
        QRegExp separ("[(]|,|[)]");
        QStringList parts = QString::fromStdString(CppString2stdString(v)).split(separ);
        if(parts.last().isEmpty())
            parts.removeLast();
        PyObject* list = newPyTuple(6);
        bool ok1,ok2,ok3, ok4, ok5=true;
        if ( parts.size() >= 5){
            setTupleItem(list, 0, PyBuildDouble(parts[1].toDouble(&ok1)));
            setTupleItem(list, 1, PyBuildDouble(parts[2].toDouble(&ok2)));
            setTupleItem(list, 2, PyBuildDouble(parts[3].toDouble(&ok3)));
            setTupleItem(list, 3, PyBuildDouble(parts[4].toDouble(&ok4)));
            parts.size()== 6 ? setTupleItem(list, 4, PyBuildDouble(parts[5].toDouble(&ok5))) : rUNDEF;
            if(! (ok1 && ok2 && ok3 && ok4 && ok5))
                return Color();

            ColorRange* helper = new ColorRange();
            std::string type = (parts[0].toLower()).toStdString();
            colortype = helper->stringToColorModel(type);

            return Color(colortype, list);
        }
    }
    return Color();
}

/*Color ColorRange::toColor(quint64 clrint, ColorModel clrModel){
    int enumVal = clrModel;
    Ilwis::ColorRange::ColorModel ilwModel = static_cast<Ilwis::ColorRange::ColorModel>(enumVal);
    QColor ilwCol = static_cast<Ilwis::ColorRange*>(_range.get())->toColor(clrint, ilwModel);

    ColorRange* colRan = new ColorRange();
    Ilwis::ContinousColorRange* ilwRan = new Ilwis::ContinousColorRange();
    Ilwis::ColorRange::ColorModel ilwMod = static_cast<const Ilwis::ColorRange*>(_range.get())->defaultColorModel();

    std::string colStr = (static_cast<const Ilwis::ColorRange*>(ilwRan)->toString(ilwCol, ilwMod)).toStdString();
    ColorModel type = static_cast<ColorModel>(ilwMod);
    return colRan->toColor(PyBuildString(colStr),type);
}*/

std::string ColorRange::toString(const Color &clr, ColorModel clrType)
{
    QString colors;
    switch(clrType){
    case ColorModel::cmCYMKA:
        colors += QString("CMYKA(%1,%2,%3,%4,%5)").arg(clr.getItem("cyan")).arg(clr.getItem("magenta")).arg(clr.getItem("yellow")).arg(clr.getItem("black")).arg(clr.getItem("alpha"));
        break;
    case ColorModel::cmHSLA:
        colors += QString("HSLA(%1,%2,%3,%4)").arg(clr.getItem("hue")).arg(clr.getItem("saturation")).arg(clr.getItem("lightness")).arg(clr.getItem("alpha"));
        break;
    case ColorModel::cmRGBA:
        colors += QString("RGBA(%1,%2,%3,%4)").arg(clr.getItem("red")).arg(clr.getItem("blue")).arg(clr.getItem("green")).arg(clr.getItem("alpha"));
        break;
    case ColorModel::cmGREYSCALE:
        break;
    }
    return colors.toStdString();
}

ColorModel ColorRange::stringToColorModel(std::string clrmd){
    if(clrmd == "rgba"){
        return ColorModel::cmRGBA;
    } else if(clrmd == "cmyka"){
        return ColorModel::cmCYMKA;
    } else if(clrmd == "hsla"){
        return ColorModel::cmHSLA;
    } else if(clrmd == "greyscale"){
        return ColorModel::cmGREYSCALE;
    }else{
        return ColorModel::cmRGBA;
    }
}

//--------------------------------------------------------------------------------

ContinousColorRange::ContinousColorRange() : ColorRange(itCONTINUOUSCOLOR,ColorModel::cmRGBA)
{
}

ContinousColorRange::ContinousColorRange(const Color &clr1, const Color &clr2, ColorModel colormodel)
{
    ColorRange* colRan = new ColorRange();
    Ilwis::ContinousColorRange* ilwRan = new Ilwis::ContinousColorRange();
    QString color1 = QString::fromStdString(colRan->toString(clr1, colormodel));
    QString color2 = QString::fromStdString(colRan->toString(clr2, colormodel));

    int enumVal = colormodel;
    Ilwis::ColorRange::ColorModel ilwColor = static_cast<Ilwis::ColorRange::ColorModel>(enumVal);

    QColor col1 = ilwRan->toColor(QVariant(color1), ilwColor);
    QColor col2 = ilwRan->toColor(QVariant(color2), ilwColor);
     _range.reset(new Ilwis::ContinousColorRange(col1, col2, ilwColor));
}



std::string ContinousColorRange::toString() const
{
    QString colors = static_cast<Ilwis::ContinousColorRange*>(_range.get())->toString();
    return colors.toStdString();
}



bool ContinousColorRange::isValid() const
{
    bool yesno = _range.get()->isValid();
    return yesno;
}


ContinousColorRange *ContinousColorRange::clone() const
{
    ContinousColorRange* colRa = new ContinousColorRange();
    colRa->_range.reset(this->_range.get()->clone());
    return colRa;
}

PyObject* ContinousColorRange::ensure(const PyObject *v, bool inclusive) const
{
    std::unique_ptr<QVariant> qvar(PyObject2QVariant(v));
    QVariant result = static_cast<Ilwis::ContinousColorRange*>(_range.get())->ensure(*qvar, inclusive);
    return QVariant2PyObject(result);
}

bool ContinousColorRange::containsVar(const PyObject *v, bool inclusive) const
{
    std::unique_ptr<QVariant> qvar(PyObject2QVariant(v));

    return _range.get()->contains(*qvar, inclusive);

}

bool ContinousColorRange::containsColor(const Color clr, bool inclusive) const
{
    ColorRange* colRan = new ColorRange();
    Ilwis::ContinousColorRange* ilwRan = new Ilwis::ContinousColorRange();
    QString color = QString::fromStdString(colRan->toString(clr, defaultColorModel()));

    Ilwis::ColorRange::ColorModel ilwColor = static_cast<const Ilwis::ColorRange*>(_range.get())->defaultColorModel();

    QColor col = ilwRan->toColor(QVariant(color), ilwColor);

    return _range.get()->contains(col, inclusive);

}

bool ContinousColorRange::containsRange(ColorRange *v, bool inclusive) const
{
    return static_cast<const Ilwis::ContinousColorRange*>(_range.get())->contains(static_cast<Ilwis::ColorRange*>(_range.get()));
}

Color ContinousColorRange::impliedValue(const PyObject *v) const
{
    std::unique_ptr<QVariant> qvar(PyObject2QVariant(v));
    QVariant colVar = static_cast<const Ilwis::ContinousColorRange*>(_range.get())->impliedValue(*qvar);
    QColor ilwCol = colVar.value<QColor>();

    ColorRange* colRan = new ColorRange();
    Ilwis::ContinousColorRange* ilwRan = new Ilwis::ContinousColorRange();
    Ilwis::ColorRange::ColorModel ilwMod = static_cast<const Ilwis::ColorRange*>(_range.get())->defaultColorModel();

    std::string colStr = (static_cast<const Ilwis::ColorRange*>(ilwRan)->toString(ilwCol, ilwMod)).toStdString();
    ColorModel type = static_cast<ColorModel>(ilwMod);
    return colRan->toColor(PyBuildString(colStr),type);

    return Color();

}
