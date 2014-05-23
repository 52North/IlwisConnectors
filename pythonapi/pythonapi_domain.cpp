#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/numericdomain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "interval.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "colordomain.h"
#include "coloritem.h"
#include "textdomain.h"
#include "range.h"

#include "pythonapi_pyobject.h"
#include "pythonapi_range.h"
#include "pythonapi_domain.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_error.h"


using namespace pythonapi;

Domain::Domain(){
}

Domain::Domain(Ilwis::IDomain *domain) : IlwisObject(new Ilwis::IIlwisObject(*domain)){
    delete domain;
}

Domain::~Domain(){
}

bool Domain::isStrict() const{
    return this->ptr()->as<Ilwis::Domain>()->isStrict();
}

void Domain::setStrict(bool yesno){
    this->ptr()->as<Ilwis::Domain>()->setStrict(yesno);
}

PyObject* Domain::impliedValue(PyObject *value) const{
    return value;
}

Domain Domain::parent() const{
    Ilwis::IDomain *domain = new Ilwis::IDomain();
    if(this->ptr()->as<Ilwis::Domain>()->parent().isValid()){
        unsigned long long id = this->ptr()->as<Ilwis::Domain>()->parent()->id();
        (*domain).prepare(id);
        return Domain( domain);
    } else {
        throw Ilwis::ErrorObject(QString("No parent domain found"));
    }
}

void Domain::setParent(const Domain &dom){
    this->ptr()->as<Ilwis::Domain>()->setParent(dom.ptr()->as<Ilwis::Domain>());
}

std::string Domain::contains(PyObject* value) const{
    std::unique_ptr<QVariant> var(PyObject2QVariant(value));
    Domain::Containement dc = (Domain::Containement)this->ptr()->as<Ilwis::Domain>()->contains(*var);
    switch(dc){
    case 1: return "cSELF";
                break;
    case 2: return "cPARENT";
                break;
    case 3: return "cDECLARED";
                break;
    case 0: return "cNONE";
        break;
    default: return "Not found anywhere";
    }
}

bool Domain::isCompatibleWith(const Domain &dom) const{
    return this->ptr()->as<Ilwis::Domain>()->isCompatibleWith(dom.ptr()->as<Ilwis::Domain>());
}

void Domain::setRange(const Range &rng){
    return this->ptr()->as<Ilwis::Domain>()->range(rng._range->clone());
}

IlwisTypes Domain::valueType() const
{
    return this->ptr()->as<Ilwis::Domain>()->valueType();
}

Domain *Domain::toDomain(Object *obj){
    Domain* ptr = dynamic_cast<Domain*>(obj);
    if(!ptr)
        throw InvalidObject("cast to Domain not possible");
    return ptr;
}

//--------------------------------------------------------------

NumericDomain::NumericDomain(const std::string &resource){
    Ilwis::INumericDomain numdom(QString::fromStdString(resource), itNUMERICDOMAIN);
    if (numdom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(numdom));
}

NumericDomain::NumericDomain(const Range &rng){
    Ilwis::INumericDomain numdom;
    numdom.prepare();
    if (numdom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(numdom));
    this->setRange(rng);
}

NumericDomain *NumericDomain::toNumericDomain(Object *obj){
    NumericDomain* ptr = dynamic_cast<NumericDomain*>(obj);
    if(!ptr)
        throw InvalidObject("cast to NumericDomain not possible");
    return ptr;
}

NumericDomain::NumericDomain(Ilwis::INumericDomain *domain): Domain(new Ilwis::IDomain(*domain)){
    delete domain;
}

//---------------------------------------------------------------

ItemDomain::ItemDomain(){
}

ItemDomain::ItemDomain(const Range& rng){
    IlwisTypes vt = rng.valueType();
    switch (vt) {
    case itNUMERICITEM:{
        Ilwis::IIntervalDomain interdom;
        interdom.prepare();
        if (interdom.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(interdom));
        setRange(rng);
    }
        break;
    case itTHEMATICITEM:{
        Ilwis::IThematicDomain themdom;
        themdom.prepare();
        if(themdom.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(themdom));
        setRange(rng);
    }
        break;
    case itNAMEDITEM:{
        Ilwis::INamedIdDomain namdom;
        namdom.prepare();
        if(namdom.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(namdom));
        setRange(rng);
    }
        break;
    case itINDEXEDITEM:
    {
    }
        break;
    default:
        break;
    }
}

quint32 ItemDomain::count()
{
    return this->ptr()->as<Ilwis::IntervalDomain>()->count();
}

void ItemDomain::setTheme(const std::string &theme)
{
    this->ptr()->as<Ilwis::IntervalDomain>()->setTheme(QString::fromStdString(theme));
}

std::string ItemDomain::theme()
{
    QString theme = this->ptr()->as<Ilwis::IntervalDomain>()->theme();
    return theme.toStdString();
}

void ItemDomain::removeItem(const std::string& nme)
{
    IlwisTypes vt = valueType();

    switch (vt) {
    case itNUMERICITEM:{
        this->ptr()->as<Ilwis::IntervalDomain>()->removeItem(QString::fromStdString(nme));
    }
        break;
    case itTHEMATICITEM:{
        this->ptr()->as<Ilwis::ThematicDomain>()->removeItem(QString::fromStdString(nme));
    }
        break;
    case itNAMEDITEM:{
        this->ptr()->as<Ilwis::NamedIdDomain>()->removeItem(QString::fromStdString(nme));
    }
        break;
    case itINDEXEDITEM:
    {
    }
        break;
    default:
        break;
    }
}

void ItemDomain::addItem(PyObject* item)
{ 
    IlwisTypes vt = valueType();

    switch (vt) {
    /*case itNUMERICITEM:{
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

            this->ptr()->as<Ilwis::IntervalDomain>()->addItem(numitem);
            }
        }*/
        break;
    case itTHEMATICITEM:{
        if ( CppTupleElementCount(item) > 0 && CppTupleElementCount(item) <=3 ) {
            QString description = sUNDEF, code = sUNDEF;
            QString label = QString::fromStdString(CppTupleElement2String(item,0));
            if ( label == sUNDEF)
                return;
            if ( CppTupleElementCount(item) >= 2){
                code = QString::fromStdString(CppTupleElement2String(item,1));
            }
            if ( CppTupleElementCount(item) == 3){
                description = QString::fromStdString(CppTupleElement2String(item,2));
            }
            Ilwis::ThematicItem *titem = new Ilwis::ThematicItem({label, code, description});

            this->ptr()->as<Ilwis::ThematicDomain>()->addItem(titem);
        }
    }
        break;
    case itNAMEDITEM:{
        QString name = QString::fromStdString(CppString2stdString(item));
        Ilwis::NamedIdentifier *id = new Ilwis::NamedIdentifier(name);
        this->ptr()->as<Ilwis::NamedIdDomain>()->addItem(id);
    }
        break;
    case itINDEXEDITEM:
    {
    }
        break;
    default:
        break;
    }
}

PyObject *ItemDomain::item(int index, bool labelOnly)
{
    Ilwis::SPDomainItem domitem = this->ptr()->as<Ilwis::IntervalDomain>()->item(index);
    IlwisTypes vt = valueType();
    switch (vt) {
    case itNUMERICITEM:{
        Ilwis::Interval *numitem = domitem->toType<Ilwis::Interval>();
        PyObject* list;
        if ( labelOnly)
            list = newPyTuple(1);
        else
            list = newPyTuple(4);
        setTupleItem(list, 0, PyUnicodeFromString(numitem->name().toLatin1()));
        if (!labelOnly){
            setTupleItem(list, 1, PyFloatFromDouble(numitem->range().min()));
            setTupleItem(list, 2, PyFloatFromDouble(numitem->range().max()));
            setTupleItem(list, 3, PyFloatFromDouble(numitem->range().resolution()));
        }
        return list;
    }
        break;
    case itTHEMATICITEM:{
        Ilwis::ThematicItem *titem = domitem->toType<Ilwis::ThematicItem>();
        PyObject* list;
        if ( labelOnly)
            list = newPyTuple(1);
        else
            list = newPyTuple(3);
        setTupleItem(list, 0, PyUnicodeFromString(titem->name().toLatin1()));
        if(!labelOnly){
            setTupleItem(list, 1, PyUnicodeFromString(titem->code().toLatin1()));
            setTupleItem(list, 2, PyUnicodeFromString(titem->description().toLatin1()));
        }
        return list;
    }
        break;
    case itNAMEDITEM:{
        PyObject* list = newPyTuple(1);
        setTupleItem(list, 0, PyUnicodeFromString(domitem->name().toLatin1()));
        return list;
    }
        break;
    case itINDEXEDITEM:
    {

    }
        break;
    default:
        break;
    }
    return 0;
}

//-----------------------------------------------------------------------------------------

ColorDomain::ColorDomain()
{
}

ColorDomain::ColorDomain(const Range& rng){
    Ilwis::IColorDomain coldom;
    coldom.prepare();
    if (coldom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(coldom));
    this->setRange(rng);
}

ColorDomain::ColorDomain(const std::string &resource)
{
    Ilwis::IColorDomain coldom(QString::fromStdString(resource), itCOLORDOMAIN);
    if (coldom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(coldom));
}

IlwisTypes ColorDomain::ilwisType() const
{
    return itCOLORDOMAIN;
}

std::string ColorDomain::containsColor(const Color &value) const{
    ColorRange* pyHelper = new ColorRange();
    Ilwis::ContinousColorRange* ilwHelper = new Ilwis::ContinousColorRange();
    std::string pyCol = pyHelper->toString(value, value.getColorModel());

    Ilwis::ColorRange::ColorModel ilwMod = static_cast<Ilwis::ColorRange::ColorModel>(value.getColorModel());

    QColor ilwCol = ilwHelper->toColor(QString::fromStdString(pyCol), ilwMod);
    Domain::Containement dc = (Domain::Containement)this->ptr()->as<Ilwis::ColorDomain>()->contains(ilwCol);
    switch(dc){
    case 1: return "cSELF";
                break;
    case 2: return "cPARENT";
                break;
    case 3: return "cDECLARED";
                break;
    case 0: return "cNONE";
        break;
    default: return "Not found anywhere";
    }
}

//--------------------------------------------------------------------------------------------

TextDomain::TextDomain()
{
}

TextDomain::TextDomain(const std::string &resource)
{
    Ilwis::ITextDomain texdom(QString::fromStdString(resource), itTEXTDOMAIN);
    if (texdom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(texdom));

}

//---------------------------------------------------------------------------------------------


TimeDomain::TimeDomain()
{
}

TimeDomain::TimeDomain(const std::string& resource)
{
    Ilwis::ITimeDomain timedom(QString::fromStdString(resource), itNUMERICDOMAIN);
    if (timedom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(timedom));
}

TimeDomain::TimeDomain(const Range& rng)
{
    Ilwis::ITimeDomain timedom;
    timedom.prepare();
    if (timedom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(timedom));
    this->setRange(rng);
}
