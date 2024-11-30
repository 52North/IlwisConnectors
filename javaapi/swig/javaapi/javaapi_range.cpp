#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/util/range.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domainitem.h"
#include "../../IlwisCore/core/ilwisobjects/domain/itemdomain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/itemrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/interval.h"
#include "../../IlwisCore/core/ilwisobjects/domain/identifieritem.h"
#include "../../IlwisCore/core/ilwisobjects/domain/thematicitem.h"
#include "../../IlwisCore/core/ilwisobjects/domain/intervalrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/identifierrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/colorrange.h"
#include "../../IlwisCore/core/ilwisobjects/domain/coloritem.h"
#include "../../IlwisCore/core/ilwisobjects/domain/rangeiterator.h"

#include "javaapi_range.h"

#include "javaapi_error.h"

#define BIGTIME 1e150

using namespace javaapi;

Range::Range(){
}

Range::~Range(){
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

QVariant* Range::ensure(const QVariant* value, bool inclusive) const
{
   std::unique_ptr<const QVariant> v(value);
   return new QVariant( _range->ensure(*v, inclusive) ); // TODO delete
   //return &_range->ensure(*v, inclusive);
}

bool Range::isContinuous() const
{
    return _range->isContinuous();
}

QVariant* Range::impliedValue(const QVariant* value) const {
    std::unique_ptr<const QVariant> v(value);
    return new QVariant( _range->impliedValue(*v) ); // TODO delete
    //return & _range->impliedValue(*v);
}

bool Range::contains(const QVariant* value, bool inclusive) const
{
   std::unique_ptr<const QVariant> v(value);
   return _range->contains(*v, inclusive);
}

//----------------------------------------------------------------------------
NumericRange::NumericRange(double mi, double ma, double resolution)
{
    _range.reset(new Ilwis::NumericRange(mi, ma, resolution))    ;
}

NumericRange::NumericRange(const NumericRange &vr) : NumericRange(vr.min(), vr.max(), vr.resolution())
{
}

NumericRange::NumericRange(Ilwis::NumericRange* nr){
    _range.reset(nr);
}

NumericRange::NumericRange(){
    _range.reset(new Ilwis::NumericRange());
}

NumericRange::~NumericRange(){

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
    return static_cast<const Ilwis::NumericRange*>(_range.get())->min();
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

NumericRangeIterator NumericRange::__iter__(){
    return NumericRangeIterator(this);
}

NumericRangeIterator NumericRange::begin(){
    Ilwis::NumericRange* ilwRng = static_cast<Ilwis::NumericRange*>(_range.get());
    Ilwis::NumericRangeIterator* ilwIt = new Ilwis::NumericRangeIterator(Ilwis::begin(*ilwRng));
    NumericRangeIterator pyIt(ilwIt);
    pyIt.setRange(this);
    return pyIt;
}

NumericRangeIterator NumericRange::end(){
    Ilwis::NumericRange* ilwRng = static_cast<Ilwis::NumericRange*>(_range.get());
    Ilwis::NumericRangeIterator* ilwIt = new Ilwis::NumericRangeIterator(Ilwis::end(*ilwRng));
    NumericRangeIterator pyIt(ilwIt);
    pyIt.setRange(this);
    return pyIt;
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

quint32 ItemRange::count()
{
    return static_cast<Ilwis::ItemRange*>(_range.get())->count();
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


void NumericItemRange::add(const std::string &name, double min, double max, double resolution){

    QString label = QString::fromStdString(name);
    Ilwis::Interval *numitem;
    if ( label == sUNDEF)
        return;
    if(resolution == 0){
        numitem = new Ilwis::Interval(label, { min, max});
    } else {
        numitem = new Ilwis::Interval(label, { min, max, resolution});
    }
    static_cast<Ilwis::ItemRange*>(_range.get())->add(numitem);
}

std::vector<std::vector<std::string> > NumericItemRange::listAll(){
    int entries = this->count();
    std::vector<std::vector<std::string> > vector;
    for(int i=0; i < entries; i++){
        Ilwis::DomainItem* item = static_cast<Ilwis::IntervalRange*>(_range.get())->item(i).data();

        std::vector<std::string> vec;
        vec.push_back( static_cast<Ilwis::Interval*>(item)->name().toStdString() );
        vec.push_back( static_cast<Ilwis::Interval*>(item)->range().toString().toStdString() );
        vector.push_back(vec);
    }
    return vector;
}

double NumericItemRange::index(double v)
{
    return static_cast<Ilwis::IntervalRange*>(_range.get())->index(v);
}

qint32 NumericItemRange::gotoIndex(qint32 index, qint32 step) const
{
    return static_cast<Ilwis::IntervalRange*>(_range.get())->gotoIndex(index, step);
}

NumericItemRange *NumericItemRange::clone()
{
    NumericItemRange* newRange = new NumericItemRange();
    newRange->_range.reset(static_cast<Ilwis::IntervalRange*>(this->_range.get()));
    return newRange;
}

//------------------------------------------------------------
/*
IndexedItemRange::IndexedItemRange()
{
    _range.reset(new Ilwis::IndexedIdentifierRange());
}

void IndexedItemRange::add(const std::string &label){
    QString qlabel = QString::fromStdString(label);
    Ilwis::IndexedIdentifier* ident = new Ilwis::IndexedIdentifier(qlabel);
    static_cast<Ilwis::IndexedIdentifierRange*>(_range.get())->add(ident);
}

void IndexedItemRange::add(const std::string &label, int index){
    QString qlabel = QString::fromStdString(label);
    Ilwis::IndexedIdentifier* ident = new Ilwis::IndexedIdentifier(qlabel, index);
    static_cast<Ilwis::IndexedIdentifierRange*>(_range.get())->add(ident);
}

void IndexedItemRange::add(const std::string &label, int index, int count){
    QString qlabel = QString::fromStdString(label);
    Ilwis::IndexedIdentifier* ident = new Ilwis::IndexedIdentifier(qlabel, index, count);
    static_cast<Ilwis::IndexedIdentifierRange*>(_range.get())->add(ident);
}

qint32 IndexedItemRange::gotoIndex(qint32 index, qint32 step) const
{
    return static_cast<Ilwis::IndexedIdentifierRange*>(_range.get())->gotoIndex(index, step);
}

IndexedItemRange *IndexedItemRange::clone()
{
    IndexedItemRange* newRange = new IndexedItemRange();
    newRange->_range.reset(static_cast<Ilwis::IndexedIdentifierRange*>(this->_range.get()));
    return newRange;
}
*/
//------------------------------------------------------------

NamedItemRange::NamedItemRange()
{
    _range.reset(new Ilwis::NamedIdentifierRange());
}

void NamedItemRange::add(const std::string& item)
{
    QString name = QString::fromStdString(item);
    Ilwis::NamedIdentifier *id = new Ilwis::NamedIdentifier(name);
    static_cast<Ilwis::ItemRange*>(_range.get())->add(id);

}

std::vector<std::string> NamedItemRange::listAll(){
    int entries = this->count();
    std::vector<std::string> vector(entries);
    for(int i=0; i < entries; i++){
        Ilwis::DomainItem* item = static_cast<Ilwis::NamedIdentifierRange*>(_range.get())->item(i).data();
        vector[i] = static_cast<Ilwis::NamedIdentifier*>(item)->name().toStdString();
    }
    return vector;
}

qint32 NamedItemRange::gotoIndex(qint32 index, qint32 step) const
{
    return static_cast<Ilwis::NamedIdentifierRange*>(_range.get())->gotoIndex(index, step);
}

NamedItemRange *NamedItemRange::clone()
{
    NamedItemRange* newRange = new NamedItemRange();
    newRange->_range.reset(static_cast<Ilwis::NamedIdentifierRange*>(this->_range.get()));
    return newRange;
}

//-----------------------------------------------------------

ThematicRange::ThematicRange()
{
    _range.reset(new Ilwis::ThematicRange());
}

void ThematicRange::add(const std::string &name, const std::string &id, const std::string &descr){
    QString description = sUNDEF, code = sUNDEF;
    QString label = QString::fromStdString(name);

    if(label == sUNDEF)
        return;
    if(id != "")
        code = QString::fromStdString(id);
    if(descr != "")
        description = QString::fromStdString(descr);

    Ilwis::ThematicItem *titem = new Ilwis::ThematicItem({label, code, description});
    static_cast<Ilwis::ItemRange*>(_range.get())->add(titem);
}

std::vector<std::vector<std::string> > ThematicRange::listAll() {
    int entries = this->count();
    std::vector<std::vector<std::string> > vector;
    for(int i=0; i < entries; ++i){
        Ilwis::DomainItem* item = static_cast<Ilwis::ThematicRange*>(_range.get())->item(i).data();

        std::vector<std::string> vec;
        vec.push_back( static_cast<Ilwis::ThematicItem*>(item)->name().toStdString() );
        vec.push_back( static_cast<Ilwis::ThematicItem*>(item)->code().toStdString() );
        vec.push_back( static_cast<Ilwis::ThematicItem*>(item)->description().toStdString() );

        vector.push_back(vec);
    }
    return vector;
}

ThematicRange *ThematicRange::clone() {
    ThematicRange* newRange = new ThematicRange();
    newRange->_range.reset(static_cast<Ilwis::ThematicRange*>(this->_range.get()));
    return newRange;
}


//-----------------------------------------------------------------

ColorRange::ColorRange(){

}

ColorRange::ColorRange(IlwisTypes tp, ColorModel clrmodel)
{
    /*int enumVal = clrmodel;
    Ilwis::ColorRangeBase::ColorModel ilwCol = static_cast<Ilwis::ColorRangeBase::ColorModel>(enumVal);
    _range.reset(Ilwis::ColorRangeBase(tp, ilwCol));*/
}

ColorModel ColorRange::defaultColorModel() const
{
    int enumVal = static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->defaultColorModel();
    ColorModel colormodel = static_cast<ColorModel>(enumVal);
    return colormodel;
}

void ColorRange::defaultColorModel(ColorModel m)
{
    int enumVal = m;
    Ilwis::ColorRangeBase::ColorModel ilwCol = static_cast<Ilwis::ColorRangeBase::ColorModel>(enumVal);
    static_cast<Ilwis::ContinuousColorRange*>(_range.get())->defaultColorModel(ilwCol);
}

Color ColorRange::toColor(const std::vector<double> &v, ColorModel colortype, const std::string& name) {
    return Color(colortype, v);
}

Color ColorRange::toColor(const std::string &v, ColorModel colortype, const std::string& name) {
    QRegExp separ("[(]|,| |[)]");
    QStringList parts = QString::fromStdString(v).split(separ);
    if(parts.last().isEmpty())
        parts.removeLast();
    std::vector<double> vec;
    bool ok = true, okI;
    if ( parts.size() >= 5) {
        vec.push_back(parts[1].toDouble(&okI)); ok &= okI;
        vec.push_back(parts[2].toDouble(&okI)); ok &= okI;
        vec.push_back(parts[3].toDouble(&okI)); ok &= okI;
        vec.push_back(parts[4].toDouble(&okI)); ok &= okI;
        if (parts.size() == 6) {
            vec.push_back(parts[5].toDouble(&okI)); ok &= okI;
        }
    }
    if ( !ok )
        return Color();

    ColorRange helper = ColorRange();
    std::string type = (parts[0].toLower()).toStdString();
    colortype = helper.stringToColorModel(type);

    return Color(colortype, vec, name);
}

Color ColorRange::toColor(quint64 clrint, ColorModel clrModel, const std::string& name){
    Ilwis::ColorRangeBase::ColorModel ilwModel = static_cast<Ilwis::ColorRangeBase::ColorModel>(clrModel);
    QColor ilwCol = Ilwis::ColorRangeBase::toColor(clrint, ilwModel);

    std::string colStr = (Ilwis::ColorRangeBase::toString(ilwCol, ilwModel)).toStdString();
    Color result = ColorRange::toColor((colStr),clrModel, name);
    return result;
}

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
        colors += QString("RGBA(%1,%2,%3,%4)").arg(clr.getItem("red")).arg(clr.getItem("green")).arg(clr.getItem("blue")).arg(clr.getItem("alpha"));
        break;
    case ColorModel::cmGREYSCALE:
        break;
    case ColorModel::cmNONE:
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

Color ColorRange::qColorToColor(QColor qCol, const std::string& name) const{
    Ilwis::ColorRangeBase::ColorModel ilwMod = static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->defaultColorModel();
    std::string colStr = (Ilwis::ColorRangeBase::toString(qCol, ilwMod)).toStdString();
    ColorModel type = static_cast<ColorModel>(ilwMod);
    return toColor((colStr),type, name);
}

QColor ColorRange::colorToQColor(const Color& pyCol) const{
    QString colorStr = QString::fromStdString(pyCol.toString());
    Ilwis::ColorRangeBase::ColorModel ilwColor = static_cast<Ilwis::ColorRangeBase::ColorModel>(pyCol.getColorModel());
    return Ilwis::ContinuousColorRange::toColor(QVariant(colorStr), ilwColor);
}

//--------------------------------------------------------------------------------

ContinuousColorRange::ContinuousColorRange()
{
    _range.reset(new Ilwis::ContinuousColorRange());
}

ContinuousColorRange::ContinuousColorRange(const Color &clr1, const Color &clr2)
{
    QString color1 = QString::fromStdString(clr1.toString());
    QString color2 = QString::fromStdString(clr2.toString());
    Ilwis::ColorRangeBase::ColorModel ilwColor = static_cast<Ilwis::ColorRangeBase::ColorModel>(clr1.getColorModel());

    QColor col1 = Ilwis::ContinuousColorRange::toColor(QVariant(color1), ilwColor);
    QColor col2 = Ilwis::ContinuousColorRange::toColor(QVariant(color2), ilwColor);
     _range.reset(new Ilwis::ContinuousColorRange(col1, col2, ilwColor));
}

ContinuousColorRange *ContinuousColorRange::clone() const
{
    ContinuousColorRange* colRa = new ContinuousColorRange();
    colRa->_range.reset(this->_range.get()->clone());
    return colRa;
}

QVariant* ContinuousColorRange::ensure(const QVariant* v, bool inclusive) const
{
    std::unique_ptr<const QVariant> qvar((v));
    QVariant result = static_cast<Ilwis::ContinuousColorRange*>(_range.get())->ensure(*qvar, inclusive);
    return new QVariant(result); //TODO delete
}

bool ContinuousColorRange::containsVar(const QVariant* v, bool inclusive) const
{
    std::unique_ptr<const QVariant> qvar((v));

    return _range.get()->contains(*qvar, inclusive);
}

bool ContinuousColorRange::containsColor(const Color &clr, bool inclusive) const
{
    QString color = QString::fromStdString(clr.toString());

    Ilwis::ColorRangeBase::ColorModel ilwColor = static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->defaultColorModel();

    QColor col = Ilwis::ContinuousColorRange::toColor(QVariant(color), ilwColor);

    return _range.get()->contains(col, inclusive);
}

bool ContinuousColorRange::containsRange(ColorRange *v, bool inclusive) const
{
    return static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->contains(static_cast<ContinuousColorRange*>(v)->_range.get(), inclusive);
}

/*Color ContinuousColorRange::impliedValue(const QVariant* v) const
{
    std::unique_ptr<const QVariant> qvar((v));
    QVariant colVar = static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->impliedValue(*qvar);
    QColor ilwCol = colVar.value<QColor>();

    return qColorToColor(ilwCol);
}*/


//---------------------------------------------------------------------------------------

ColorPalette::ColorPalette(){
    _range.reset(new Ilwis::ColorPalette());
}

Color ColorPalette::item(quint32 raw) const{
    Ilwis::SPDomainItem ilwItem = static_cast<const Ilwis::ColorPalette*>(_range.get())->item(raw);
    return itemToColor(ilwItem);
}

Color ColorPalette::item(const std::string& name) const{
    Ilwis::SPDomainItem ilwItem = static_cast<const Ilwis::ColorPalette*>(_range.get())->item(QString::fromStdString(name));
    return itemToColor(ilwItem);
}

Color ColorPalette::itemByOrder(quint32 raw) const{
    Ilwis::SPDomainItem ilwItem = static_cast<const Ilwis::ColorPalette*>(_range.get())->itemByOrder(raw);
    return itemToColor(ilwItem);
}

Color ColorPalette::color(int index){
    QColor ilwCol = static_cast<const Ilwis::ColorPalette*>(_range.get())->color(index);

    return qColorToColor(ilwCol);
}

void ColorPalette::add(const Color& pyColor){
    QColor ilwCol = colorToQColor(pyColor);
    Ilwis::ColorItem* ilwItem = new Ilwis::ColorItem(ilwCol);
    static_cast<Ilwis::ColorPalette*>(_range.get())->add(ilwItem);
}

void ColorPalette::remove(const std::string &name){
    static_cast<Ilwis::ColorPalette*>(_range.get())->remove(QString::fromStdString(name));
}

void ColorPalette::clear(){
    static_cast<Ilwis::ColorPalette*>(_range.get())->clear();
}

bool ColorPalette::containsColor(const Color &clr, bool inclusive) const{
    QColor ilwCol= colorToQColor(clr);
    return static_cast<Ilwis::ColorPalette*>(_range.get())->contains(QVariant(ilwCol), inclusive);
}

bool ColorPalette::containsRange(ColorRange *v, bool inclusive) const{
    return static_cast<const Ilwis::ColorPalette*>(_range.get())->contains(static_cast<ColorPalette*>(v)->_range.get(), inclusive);
}

quint32 ColorPalette::count(){
    return static_cast<const Ilwis::ColorPalette*>(_range.get())->count();
}

Color ColorPalette::valueAt(quint32 index, ItemRange *rng){
    QColor ilwCol = static_cast<const Ilwis::ColorPalette*>(_range.get())->valueAt(index, static_cast<ColorPalette*>(rng)->_range.get());
    return qColorToColor(ilwCol);
}

Color ColorPalette::itemToColor(Ilwis::SPDomainItem item) const{
    Ilwis::SPColorItem citem = item.staticCast<Ilwis::ColorItem>();

    QColor ilwCol = citem->color();
    QString name = citem->name();

    return ColorRange::qColorToColor(ilwCol, name.toStdString());
}

qint32 ColorPalette::gotoIndex(qint32 index, qint32 step) const{
    return static_cast<const Ilwis::ColorPalette*>(_range.get())->gotoIndex(index, step);
}

void ColorPalette::add(QVariant* item){
    //from superclass, so class is not abstract
}

//---------------------------------------------------------------------------------------
/*
TimeInterval::TimeInterval(IlwisTypes tp) : NumericRange(-BIGTIME, BIGTIME,0) {
    _range.reset(new Ilwis::TimeInterval(tp));
}

TimeInterval::TimeInterval(const QVariant* beg, const QVariant* end, std::string step, IlwisTypes tp):
    NumericRange(-BIGTIME, BIGTIME,0){
    const QVariant* qvar = (beg);
    Ilwis::Time ilwBeg = qvar->value<Ilwis::Time>();
    qvar = (end);

    Ilwis::Time ilwEnd = qvar->value<Ilwis::Time>();
    Ilwis::Duration ilwStep {QString::fromStdString(step)};
    _range.reset(new Ilwis::TimeInterval(ilwBeg, ilwEnd, ilwStep, tp));

    delete qvar;
}

TimeInterval::TimeInterval(Ilwis::Range* ti){
    _range.reset(static_cast<Ilwis::TimeInterval*>(ti));
}

//std::string TimeInterval::__str__(){
//    QString begin = static_cast<Ilwis::TimeInterval*>(_range.get())->begin().toString();
//    QString end = static_cast<Ilwis::TimeInterval*>(_range.get())->end().toString();
//    return "Start: " + begin.toStdString() + ", End: " + end.toStdString();
//}

bool TimeInterval::contains(const std::string &value, bool inclusive) const
{
    return static_cast<Ilwis::TimeInterval*>(_range.get())->contains(QString::fromStdString(value));
}

bool TimeInterval::contains(const QVariant* value, bool inclusive) const
{
    std::unique_ptr<const QVariant> qvar((value));
    if(qvar->value<Ilwis::Time>())
    {
        Ilwis::Time timeVal = qvar->value<Ilwis::Time>();
        return static_cast<Ilwis::TimeInterval*>(_range.get())->contains(timeVal);
    } else
        return static_cast<Ilwis::TimeInterval*>(_range.get())->contains(*qvar);
}


TimeInterval* TimeInterval::clone() const
{
    return new TimeInterval(static_cast<Ilwis::TimeInterval*>(_range.get())->clone());
}

void TimeInterval::begin(const QVariant* t)
{
    std::unique_ptr<const QVariant> qvar((t));
    Ilwis::Time timeVal = qvar->value<Ilwis::Time>();
    static_cast<Ilwis::TimeInterval*>(_range.get())->begin(timeVal);
}

void TimeInterval::end(const QVariant* t)
{
    std::unique_ptr<const QVariant> qvar((t));
    Ilwis::Time timeVal = qvar->value<Ilwis::Time>();
    static_cast<Ilwis::TimeInterval*>(_range.get())->end(timeVal);
}

QVariant* TimeInterval::begin() const
{
    QVariant* qvar = new QVariant(IVARIANT(static_cast<Ilwis::TimeInterval*>(_range.get())->begin()));
    return (qvar);
}

QVariant* TimeInterval::end() const
{
    QVariant* qvar = new QVariant(IVARIANT(static_cast<Ilwis::TimeInterval*>(_range.get())->end()));
    return (qvar);;
}
*/
