#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/raster.h"
#include "../../IlwisCore/core/util/linerasterizer.h"
#include "../../IlwisCore/core/util/tranquilizer.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/pixeliterator.h"

#include "pythonapi_pixeliterator.h"

namespace pythonapi {

PixelIterator::PixelIterator(RasterCoverage *rc): _coverage(rc), _ilwisPixelIterator(new Ilwis::PixelIterator((*rc->ptr()))){
}

double PixelIterator::next(){
    double d = (*(*this->_ilwisPixelIterator));
    this->_ilwisPixelIterator->operator ++();
    return d;
}

bool PixelIterator::hasNext(){
    return ((*this->_ilwisPixelIterator) != this->_ilwisPixelIterator->end());
}

} // namespace pythonapi
