#!/bin/sh

#TODO: path to Qt binaries and to ILWIS binaries needs to be updated on linux
echo "save old PATH"
old=$PATH

echo "add Qt- and ILWIS-path to PATH"
source prepare_PATH.sh

echo "==========start test.py=============="
python test.py
echo "==========test.py ended=============="

echo "reset PATH"
PATH=$old
