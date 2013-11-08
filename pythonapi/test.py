#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ilwisobjects import *

def main():
    muteIssueLogger()
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
        except TypeError as err:
            print("caught TypeError:",err)
        del it
        del f
        del v
        print("sum of rainfall values in may:",sum)
        del sum
    else:
        print("couldn't load FeatureCoverage")
    del fc

#    fc = FeatureCoverage("file:///C:/Users/Poku");
#    try:
#        for f in fc:
#            print(f)
#    except IlwisException as err:
#        print("caught error:",err)

    rc = RasterCoverage()
    rc.connectTo("file:///C:/Users/Poku/dev/Ilwis4/testdata/n000302.mpr")
    rc.connectTo("", "GTiff","gdal",IlwisObject.cmOUTPUT)
    if rc.store(IlwisObject.smBINARYDATA + IlwisObject.smMETADATA):
        print("successfully saved n000302.mpr.tif")
    else:
        print("could not save n000302.mpr.tif")



def claudio_example():#and martins solution proposal
#    ilwisengine = ilwisobjects.engine()
#    #create a feature coverage
     distribution = FeatureCoverage()#ilwisengine.features()
#    #link it to a local shape file with species distribution. the attributes will contain an attribute 'distribution'.
#    distribution.connectTo("file://d:/somepath/species.shp");
#    #create a coordinate system; we could also use the csy of the distribution map but lets create (for interface sake) a coordinate system
#    localcoordsystem = ilwisengine.coordinatesystem("+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
#    #a variant could be
#    # localcoordsystem = ilwisengine.coordinatesystem("code=epsg:23035")
#    #setting the bounds
#    localcoordsystem.bounds(1003700, 239900, 1004600, 2409000)
#    # create a polygon grid
#    polygongrid = ilwisengine.do("gridding", localcoordsystem,10039939, 2399393, 10045997, 2405000, 1000)
#    #add an attribute for the highest distribution ; name plus domain as parameters. Others could include ranges and such. Keep it simple here
#    polygongrid.addAttribute("highest","numeric")
#    #small trivial algorithm for detecting the highest point attribute per polygon
#    for polygon in polygongrid
#        for point in distribution
#            if polygon.contains(distribution.coordinatesystem(), point)
#                maxval = max(polygon.attribute("highest"), point.attribute("distribution"))
#                polygon.attribute("highest", maxval)

if __name__ == "__main__":
    main()
