#ifndef PYTHONAPI_FEATURE_H
#define PYTHONAPI_FEATURE_H

namespace Ilwis{
    class SPFeatureI;
}
namespace pythonapi{

class Feature{
public:
    Feature(Ilwis::SPFeatureI* ilwisFeature);
    const char* toStr();
private:
    Ilwis::SPFeatureI* _ilwisSPFeatureI;
};

}

#endif // PYTHONAPI_FEATURE_H
