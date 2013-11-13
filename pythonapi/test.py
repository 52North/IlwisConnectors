#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ilwisobjects import *

def main():
    Engine.setWorkingCatalog("file:///C:/Users/Poku/dev/Ilwis4/testdata/shape")
    #muteIssueLogger()
    print("-----------------------------------------------")
    #FeatureCoverage
    fc = FeatureCoverage("file:///C:/Users/Poku/dev/Ilwis4/testdata/shape/rainfall.shp")
    if fc:
        print("successfully loaded", fc)
        print(fc ,"contains:",fc.featureCount(),"Features")

        print("-----------------------------------------------")
        #iterating over features
        sum = 0
        for f in fc:
            sum += int(f.attribute("MAY"))
            print(f, ":", f.attribute("coverage_key"), ",", f.attribute("MAY"), ",", f.attribute("RAINFALL"));
        print("sum of rainfall values in may:",sum)
        del sum

        print("-----------------------------------------------")
        #catching native python exception
        it = fc.__iter__();
        f = it.next()
        v = f.attribute("RAINFALL")
        try:
            print(int(v))
        except TypeError as err:
            print("caught TypeError:",err)

    else:
        print("couldn't load FeatureCoverage")
    if fc.addAttribute("highest","value"):
        print("numeric attribute 'highest' was added to",fc)
    else:
        print("couln't add numeric attribute 'highest' to",fc)

    print("-----------------------------------------------")
    Engine.setWorkingCatalog("file:///C:/Users/Poku/dev/Ilwis4/testdata")
    rc = RasterCoverage()
    rc.connectTo("file:///C:/Users/Poku/dev/Ilwis4/testdata/n000302.mpr")
    if (rc):
        print("successfully loaded",rc)
        print(rc,".value(342,342,0)=>",rc.value(342,342,0))
        aa7 = Engine.do("aa7.mpr=sin(n000302.mpr)")
        print("sin(n000302.mpr)=>",aa7)
        print(aa7,".value(342,342,0)=>",aa7.value(342,342,0))
        print("-----------------------------------------------")
        print(rc,".value(342,342,0)=>",rc.value(342,342,0))
        aa1 = rc + rc
        print(rc, " + ", rc, " = ", aa1)
        print(aa1,".value(342,342,0)=>",aa1.value(342,342,0))
        aa2 = rc + 2
        print(rc, " + 2 = ", aa2)
        print(aa2,".value(342,342,0)=>",aa2.value(342,342,0))
        aa3 = 2 + rc
        print("2 + ", rc, " = ", aa3)
        print(aa3,".value(342,342,0)=>",aa3.value(342,342,0))
        print("-----------------------------------------------")
        #store to file
        aa7.connectTo("file:///C:/Users/Poku/dev/Ilwis4/testdata/aa7.tif", "GTiff","gdal",IlwisObject.cmOUTPUT)
        if aa7.store(IlwisObject.smBINARYDATA + IlwisObject.smMETADATA):
            print("successfully saved aa7.mpr.tif")#stores .tif into C:/PATHONDIR/....
        else:
            print("could not save aa7.mpr.tif")
    else:
        print("couldn't load RasterCoverage")


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

def martin_example():
    nir = RasterCoverage("file:///C:/some/dir/nir_2010.img")
    vis = RasterCoverage("file:///C:/some/dir/vis_2010.img")
    ndvi = (nir - vis)/(nir + vis)
    daily_ndvi = ndvi.do("selection [0,0,"+str(Time("2010-4-1").double())+"] ["+str(ndvi.size().x())+","+str(ndvi.size().x())+","+str(Time("2010-4-1").double())+"]")
    dekadel_ndvi = Engine.do("aggregateraster",daily_ndvi,'Avg',[1,1,Duration(0).setDay(10).toInt()],True)
    dest_coordsys = CoordinateSystem("code=epsg:3148")
    out = dekadel_ndvi.transform(dest_coordsys)
    out.connectTo("file:///C:/some/dir/ndvi_2010-4-1_2010-7-1.netcdf","netcdf","gdal",IlwisObject.cmOUTPUT)
    out.store(IlwisObject.smBINARYDATA + IlwisObject.smMETADATA)

if __name__ == "__main__":
    main()
