#include "ilwis.h"
#include "rawconverter.h"

using namespace Ilwis;
using namespace Ilwis3;

RawConverter::RawConverter(double low, double high, double step)  {
    _storeType =  minNeededStoreType(low, high, step);
    _offset = determineOffset(low, high, step, _storeType);
    _scale = step; // determineScale(low, high, step);
    _undefined = guessUndef(low,high);

}

IlwisTypes RawConverter::minNeededStoreType(double low, double high, double step) const{
    double minDivStep;
    double maxDivStep;
    intRange(low, high, step, minDivStep, maxDivStep );

    quint64 delta = abs(maxDivStep - minDivStep);//TODO change from quint32 to quint64 might change behaviour??
    if ( step != 0) {
        if ( delta <= 255)
            return itUINT8;
        else if ( delta <= 65535)
            return itINT16;
        else if ( delta <= 4294967295){
            return itINT32;
        }
    }
    return itDOUBLE;

}

long RawConverter::rounding(double x) const {
    return (long)floor(x + 0.5);
}

void RawConverter::intRange(double low, double high, double step, double& minDivStep, double& maxDivStep) const {
    minDivStep = low;
    maxDivStep = high;
    if (step > 1e-30) {
        minDivStep /= step;
        maxDivStep /= step;
    }
}

double RawConverter::determineScale(double low, double high, double step) const  {
    double minDivStep;
    double maxDivStep;
    intRange(low, high, step, minDivStep, maxDivStep );
    int r = log10(abs(maxDivStep - minDivStep)) + 1;
    return pow(10,-r);

}

double RawConverter::determineOffset(double low, double high, double step, IlwisTypes st)
{
  if (st == itDOUBLE)
    return 0;
  double minDivStep;
  double maxDivStep;
  intRange(low, high, step, minDivStep, maxDivStep );

  double r0 = 0;
  double r = rounding(minDivStep / 2 + maxDivStep / 2 - 0.0001);
  r0 = r;
  switch (st) {
     case itINT32:
       if (minDivStep < -LONG_MAX || maxDivStep > LONG_MAX)
         r0 = minDivStep / 2 + maxDivStep / 2 - 0.0001;
       else
         r0 = 0;
       break;
     case itINT16:
       if (minDivStep < -SHRT_MAX || maxDivStep > SHRT_MAX)
         r0 = minDivStep / 2 + maxDivStep / 2 - 0.0001;
       else
         r0 = 0;
       break;
     case itUINT8:
       if (minDivStep < 0 || maxDivStep > 255)
         r0 = minDivStep-1;
       else
         r0 = -1;
       break;
  }
  return r0;
}

double RawConverter::guessUndef(double vmin, double vmax) {
    if ( vmin >= std::numeric_limits<byte>::min() && vmax <= std::numeric_limits<byte>::max())
       return 0;
    if ( vmin >= std::numeric_limits<short>::min() && vmax <= std::numeric_limits<short>::max())
       return shILW3UNDEF;
    else if ( vmin >= std::numeric_limits<long>::min() && vmax <= std::numeric_limits<long>::max())
        return iILW3UNDEF;
    if ( vmin >= std::numeric_limits<float>::min() && vmax <= std::numeric_limits<float>::max())
        return flUNDEF;
    return rUNDEF;
}

