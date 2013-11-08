#ifndef PYTHONAPI_OBJECT_H
#define PYTHONAPI_OBJECT_H

namespace pythonapi{

    class Object{
    public:
        Object(){};
        virtual ~Object(){};
        virtual bool __bool__() const = 0;
        virtual const char* __str__() = 0;
    };

}
#endif // PYTHONAPI_OBJECT_H
