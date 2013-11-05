#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ilwisobjects import *

def main():
    fc = FeatureCoverage("file:///C:/Users/Poku/dev/Ilwis4/testdata/shape/rainfall.shp")
    if fc.isValid():
        print("successfully loaded", fc)
        print(fc ,"contains:",fc.featureCount(),"Features")
        sum = 0
        for f in fc:
            sum += int(f.cell("MAY"))
            print(f, ":", f.cell("coverage_key"), ",", f.cell("MAY"), ",", f.cell("RAINFALL"));
        it = fc.__iter__();
        f = it.next()
        v = f.cell("RAINFALL")
        try:
            print(int(v))
        except(IlwisException) as err:
            print("caught error:",err)
        del v
        print("sum of rainfall values in may:",sum)
    else:
        print("couldn't load FeatureCoverage:", fc)

if __name__ == "__main__":
    main()
