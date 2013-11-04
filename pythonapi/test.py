#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ilwisobjects import *

def main():
    fc = FeatureCoverage("file:///C:/Users/Poku/dev/Ilwis4/testdata/shape/rainfall.shp")
    if fc.isValid():
        print("successfully loaded", fc)
        print(fc ,"contains:",fc.featureCount(),"Features")
        for f in fc:
            print(f, ":", f.cell("coverage_key"), ",", f.cell("MAY"), ",", f.cell("rainfall"));
    else:
        print("couldn't load FeatureCoverage:", fc)

    fc = FeatureCoverage("file:///C:/Users/Poku/dev/Ilwis4/testdata/Rainfall.mpp")
    if fc.isValid():
        print("successfully loaded", fc)
        print(fc,"contains:",fc.featureCount(),"Features")
        for f in fc:
            print(f, ":", f.cell("coverage_key"), ",", f.cell("may"), ",", f.cell("RAINFALL"));
    else:
        print("couldn't load FeatureCoverage:", fc)

if __name__ == "__main__":
    main()
