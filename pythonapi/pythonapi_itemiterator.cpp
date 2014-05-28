//#include "pythonapi_domain.h"
//#include "pythonapi_range.h"
//#include "pythonapi_itemiterator.h"

//#include "itemiterator.h"

//namespace pythonapi{

//ItemIterator::ItemIterator() : _current(0){
//    this->_ilwisItemIterator.reset(new Ilwis::ItemIterator());
//}

//ItemIterator::ItemIterator(ItemRange &rng) : _range(rng), _current(0)
//{

//    this->_ilwisItemIterator.reset(new Ilwis::ItemIterator());
//}

//Ilwis::ItemIterator& ItemIterator::ptr() const{
//    if (!this->__bool__())
//        throw InvalidObject("use of invalid ItemIterator (ptr)");
//    return (*this->_ilwisPixelIterator);
//}

//}
