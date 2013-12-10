#ifndef PYTHONAPI_PIXELITERATOR_H
#define PYTHONAPI_PIXELITERATOR_H

#include "pythonapi_rastercoverage.h"

namespace Ilwis {
    class PixelIterator;
}

namespace pythonapi {

class PixelIterator{
    public:
        PixelIterator(RasterCoverage* rc);
        /**
         * @brief next confusingly returns current value bevore iterating to the next item
         * brings together C++ style "std::iterator it != it.end()" with end() pointing the back (after last entry)
         * and Python style "for it in it:" using the __next__() also to retrieve first item.
         * @return
         */
        double& next();
        bool hasNext();
    private:
        RasterCoverage* _coverage;
        std::shared_ptr<Ilwis::PixelIterator> _ilwisPixelIterator;
};

} // namespace pythonapi

#endif // PYTHONAPI_PIXELITERATOR_H
