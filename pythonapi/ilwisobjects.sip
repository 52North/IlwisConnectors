// Define the SIP wrapper to the word library.

%Module ilwisobjects

%Import QtCore/QtCoremod.sip

class IlwisObject{

%TypeHeaderCode
        #include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
%End

public:
    IlwisObject();
};
