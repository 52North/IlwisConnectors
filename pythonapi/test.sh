#!/bin/sh

#TODO: path to Qt binaries and to ILWIS binaries needs to be updated on linux
echo "save old PATH"
old=$PATH
echo "add Qt- and ILWIS-path to PATH"
PATH=$PATH:/c/Qt/Qt5.1.1/5.1.1/mingw48_32/bin:/c/Users/Poku/dev/Ilwis4/projects/output/win32debug/bin
echo "==========start test.py=============="
python test.py
echo "==========test.py ended=============="
echo "reset PATH"
PATH=$old
