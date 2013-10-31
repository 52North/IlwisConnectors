#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ilwisobjects import *

def main():
    fc = FeatureCoverage("file:///C:/Users/Poku/dev/Ilwis4/testdata/shape/rainfall.shp")
    if fc.isValid():
        print("successfully loaded", fc.toStr())
        print(fc.toStr(),"contains:",fc.featureCount(),"Features")
    else:
        print("couldn't load FeatureCoverage:", fc.toStr())

    fc = FeatureCoverage("file:///C:/Users/Poku/dev/Ilwis4/testdata/Rainfall.mpp")
    if fc.isValid():
        print("successfully loaded", fc.toStr())
        print(fc.toStr(),"contains:",fc.featureCount(),"Features")
    else:
        print("couldn't load FeatureCoverage:", fc.toStr())

if __name__ == "__main__":
    main()
