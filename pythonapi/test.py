#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ilwisobjects import *

def main():
    io = FeatureCoverage("file://C:/Users/Poku/dev/Ilwis4/testdata/Rainfall.mpp")
    print(io.toStr())

if __name__ == "__main__":
    main()
