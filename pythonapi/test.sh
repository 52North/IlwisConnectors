#!/bin/sh

#TODO: path to Qt binaries and to ILWIS binaries needs to be updated on linux
PATH=$PATH:/c/Qt/Qt5.1.1/5.1.1/mingw48_32/bin:$ILWISDIR
python test.py
