#include "log.h"

#include "Python.h"

void pythonapi::log(const char *message){
    PySys_WriteStderr("ILWIS ERROR: %s \n",message);
}
