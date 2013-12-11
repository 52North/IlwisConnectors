#ifndef PYTHONAPI_PIXELITERATOR_H
#define PYTHONAPI_PIXELITERATOR_H

namespace Ilwis {
    class PixelIterator;
}

#include "pythonapi_qtGNUTypedefs.h"
#include "pythonapi_geometry.h"

namespace pythonapi {

class RasterCoverage;

class PixelIterator{
    public:
        PixelIterator(const PixelIterator &pi);
        PixelIterator(RasterCoverage* rc);
        /**
         * @brief next confusingly returns current value bevore iterating to the next item
         * brings together C++ style "std::iterator it != it.end()" with end() pointing the back (after last entry)
         * and Python style "for it in it:" using the __next__() also to retrieve first item.
         * @return
         */
        PixelIterator* __iter__();
        double __next__();
        bool __bool__() const;
        const char* __str__();
        /**
         * @brief __float__ returns value on current position
         * @return returns value on current position
         */
        double __float__();
        /**
         * @brief __int__ returns the current linear position of the iterator
         * @return returns the current linear position of the iterator
         */
        quint64 __int__();

        Voxel position();
        bool xChanged();
        bool yChanged();
        bool zChanged();
        PixelIterator& __getitem__(Voxel& vox);
        double __getitem__(quint32 position);
        void __setitem__(quint32 position, double value);
        PixelIterator __add__(int n);
        PixelIterator __radd__(int n);
        PixelIterator* __iadd__(int n);

        bool __eq__(const PixelIterator& other);
        bool __ne__(const PixelIterator& other);
        bool __le__(const PixelIterator& other);
        bool __lt__(const PixelIterator& other);
        bool __ge__(const PixelIterator& other);
        bool __gt__(const PixelIterator& other);
private:
        Ilwis::PixelIterator& ptr() const;
        RasterCoverage* _coverage;
        std::shared_ptr<Ilwis::PixelIterator> _ilwisPixelIterator;
};

} // namespace pythonapi

#endif // PYTHONAPI_PIXELITERATOR_H
