#!/usr/bin python
# -*- coding: utf-8 -*-

from ilwisobjects import *

workingDir = "file:///C:/Users/Poku/dev/Ilwis4/testdata"

def main():
    operation("/pytest")
    feature("/pytest")
    data("/pytest")
    coordinateSystem("/pytest")
    pixelIterator("/pytest")
    raster("/pytest")
    hello_raster("/baby")
    hello_feature("/baby")
    claudio_example("/example")

def cheese():
    c = Cheese("Gouda")
    dic = {c : "lecker"}
    print("ist",dic[c], hash(c))

def data(subDir):
    Engine.setWorkingCatalog(workingDir+subDir)
    fc = FeatureCoverage("rainfall.shp")
    testvalues = (-9223372036854775808,-9223372036854775809,18446744073709551615,18446744073709551616,0.432,23.4e-32)
    if fc:
        print("successfully loaded", fc.name())
        if fc.addAttribute("highest","value"):
            i = 0
            for f in fc:
                try:
                    f["highest"] = testvalues[i%len(testvalues)]
                    print(testvalues[i%len(testvalues)],"::",f["highest"])
                except OverflowError as e:
                    print(e)
                i += 1


#further tests on all the working api calls
def operation(subDir):
    Engine.setWorkingCatalog(workingDir+subDir)
    disconnectIssueLogger()
    connectIssueLogger()
    #Gridding
    localcoordsystem = CoordinateSystem("code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
    polygongrid = Engine.do("gridding",localcoordsystem,Coordinate(225358.6605, 3849480.5700),1000.0,1000.0,12,12)
    if polygongrid:
        print("successfully created",polygongrid.type(),"(",polygongrid.name(),") with gridding")
        print("FeatureCount is",polygongrid.featureCount())
    else:
        print("couln't create FeatureCoverage with gridding")

def feature(subDir):
    Engine.setWorkingCatalog(workingDir+subDir)
    fc = FeatureCoverage("rainfall.shp")
    if fc:
        print("successfully loaded", fc.name())
        print(fc.name() ,"contains:",fc.featureCount(),"Features")
        print("-----------------------------------------------")
        #adding new attribute to coverage
        if fc.addAttribute("highest","value"):
            print("value attribute 'highest' was added to",fc.name())
            att = fc.attributes()
            print("attributes(",len(att),")",att)
        else:
            print("couln't add value attribute 'highest' to",fc.name())
        print("-----------------------------------------------")
        #adding new feature to coverage
        it = iter(fc)#loadBinaryData
        g = Geometry("POINT(5.4 6 9.0)")
        print(g)
        newF = fc.newFeature(g);
        for c in fc.attributes():
            newF[c] = 12.0
        print("-----------------------------------------------")
        #iterating over features
        sum = 0
        for f in fc:
            sum += float(f.attribute("MAY",0))
            f["highest"] = sum
            print(f, end=":")
            print(f.geometry(), end=">")
            for a in fc.attributes():
                print(f[a], end="|")
            print()
        print("sum of rainfall values in may:",sum)
        del sum
        print("-----------------------------------------------")
        print("alter Geometry using WKT")
        f = next(it)
        print(f.geometry())
        check = False
        try:
            check = f.geometry().fromWKT(newF.geometry().toWKT())
        except Exception as a:
            print(a)
        print(check, " -> ",f.geometry())
        print("-----------------------------------------------")
        print("catching native python exception")
        try:
            v = f["RAINFAL"]
        except Exception as e:
            print(e)
            v = f["RAINFALL"]
        try:
            print(int(v))
        except TypeError as err:
            print("caught TypeError:",err)

    else:
        print("couldn't load FeatureCoverage")

def coordinateSystem(subDir):
    Engine.setWorkingCatalog(workingDir+subDir)
    fc = FeatureCoverage("rainfall.shp")
    print("CoordinateSystems")
    cs1 = CoordinateSystem("code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
    cs1.name("myProj4CSY")
    if cs1:
        fc.setCoordinateSystem(cs1)
        print(fc.coordinateSystem().name())
    else:
        print("Coudn't create CoordinateSystem from proy4 definition!")
    cs2 = CoordinateSystem("code=epsg:23035")
    if cs2:
        print(cs2.name())
    else:
        print("Coudn't create CoordinateSystem from EPSG code!")

def raster(subDir):
    Engine.setWorkingCatalog(workingDir+subDir)
    rc = RasterCoverage("n000302.mpr")
    rctif = RasterCoverage("n0.mpr")
    if (rc and rctif):
        print("successfully loaded",rc.name(),"and",rctif.name())
        print(rc.name()+".pix2value(342,342,0) =>",rc.pix2value(342,342,0))
        print(rc.name()+".coord2value(Coordinate(-1871900.47,1874237.55)) =>",rc.coord2value(Coordinate(-1871900.47,1874237.55)))
        print(rctif.name()+".pix2value(342,342,0) =>",rctif.pix2value(342,342,0))
        print("-----------------------------------------------")
        aa7 = Engine.do("sin","n000302.mpr")
        print("sin(n000302.mpr)=>",aa7.name()+".pix2value(342,342,0)=>",aa7.pix2value(342,342,0))
        print("-----------------------------------------------")
        aa1 = rc + rctif
        print(rc.name(), " + ", rctif.name(), " = ", aa1.name()+".pix2value(342,342,0)=>",aa1.pix2value(342,342,0))
        aa2 = rc + 2
        print(rc.name(), " + 2 = ", aa2.name()+".pix2value(342,342,0)=>",aa2.pix2value(342,342,0))
        aa3 = 2 + rc
        print("2 + ", rc.name(), " = ", aa3.name()+".pix2value(342,342,0)=>",aa3.pix2value(342,342,0))
        aa4 = rc - rctif
        print(rc.name(), " - ", rc.name(), " = ", aa4.name()+".pix2value(342,342,0)=>",aa4.pix2value(342,342,0))
        aa5 = 2 - rc #until now this is parsed as "rc - 2" :(
        print("2 - ", rc.name(), " = ", aa5.name()+".pix2value(342,342,0)=>",aa5.pix2value(342,342,0))
        aa6 = rc - 2
        print(rc.name(), " - 2 = ", aa6.name()+".pix2value(342,342,0)=>",aa6.pix2value(342,342,0))
        aa7 = rc / rctif
        print(rc.name(), " / ", rc.name(), " = ", aa7.name()+".pix2value(342,342,0)=>",aa7.pix2value(342,342,0))
        aa8 = 2 / rc #until now this is parsed as "rc / 2" :(
        print("2 / ", rc.name(), " = ", aa8.name()+".pix2value(342,342,0)=>",aa8.pix2value(342,342,0))
        aa9 = rc / 2
        print(rc.name(), " / 2 = ", aa9.name()+".pix2value(342,342,0)=>",aa9.pix2value(342,342,0))
        aa10 = rc * rctif
        print(rc.name(), " * ", rc.name(), " = ", aa10.name()+".pix2value(342,342,0)=>",aa10.pix2value(342,342,0))
        aa11 = 2 * rc
        print("2 * ", rc.name(), " = ", aa11.name()+".pix2value(342,342,0)=>",aa11.pix2value(342,342,0))
        aa12 = rc * 2
        print(rc.name(), " * 2 = ", aa12.name()+".pix2value(342,342,0)=>",aa12.pix2value(342,342,0))
        print("-----------------------------------------------")
        #store to file
        if aa1.connectTo(workingDir+subDir+"/aa1", "GTiff","gdal",IlwisObject.cmOUTPUT):
            if aa1.store():
                print("successfully saved aa1.tif")
            else:
                print("could not save aa1.tif")
        else:
            print("connectTo file failed!")
    else:
        print("couldn't load RasterCoverage")

def pixelIterator(subDir):
    Engine.setWorkingCatalog(workingDir+subDir)
    #iterate over pixel
    rcl = RasterCoverage("small.mpl")
    if rcl:
        for v in rcl:
            print(v,end="|")
        print()
        rit = iter(rcl)
        if rit:
            print("successfully created",rit)
            rit += 4
            rit2 = rit + 3
            rit3 = 13 + rit
            print(rit3.position(), "==", rit2.position(),"=>",rit3 == rit2)
            print("float(iter(small.mpl)+=1)=>",float(rit2))
            print("iter(small.mpl)[23]=>",rit[23])
            print("float(iter(small.mpl))=>",float(rit))
            print("iter(small.mpl)[23]=434")
            rit[23] = 434
            print("iter(small.mpl)[23]=>",rit[23])
            rit2 = PixelIterator(rit[Voxel(2,2,2)])
            print("next(iter(small.mpl)[",rit.position(),"==",int(rit),"])=>",next(rit))
            print(rit.position(),rit.xChanged(),rit.yChanged(),rit.zChanged())
            print(rit.position(), "==", rit2.position(),"=>",rit == rit2)
            print(rit.position(), "!=", rit2.position(),"=>",rit != rit2)
            print(rit.position(), ">=", rit2.position(),"=>",rit >= rit2)
            print(rit.position(), ">", rit2.position(),"=>",rit > rit2)
            print(rit.position(), "<=", rit2.position(),"=>",rit <= rit2)
            print(rit.position(), "<", rit2.position(),"=>",rit < rit2)
            print("float(iter(small.mpl)[",rit.position(),"==",int(rit),"])=>",float(rit))


def claudio_example(subDir):#and martins solution proposal <== example code for presentation
    Engine.setWorkingCatalog(workingDir+subDir)

    distribution = FeatureCoverage(workingDir+subDir+"/freq.mpp")
    polygongrid = Engine.do("gridding", distribution.coordinateSystem(),Coordinate(26.5,4.5),1,1,15,13)
    print(polygongrid.name())
    polygongrid.addAttribute("maxY","value")
    for polygon in polygongrid:
#        polygon.setAttribute("maxY", 0)
        for point in distribution:
            if polygon.geometry().contains(point.geometry()):
                maxval = max(int(polygon.attribute("maxY",0)), int(point.attribute("freq_speciesY",0)))
                polygon.setAttribute("maxY", maxval)

    polygongrid.connectTo(workingDir+subDir+"/polygongrid", "polygonmap", "ilwis3", IlwisObject.cmOUTPUT)
    polygongrid.store()
    polygongrid.connectTo(workingDir+subDir+"/polygongrid", "ESRI Shapefile", "gdal", IlwisObject.cmOUTPUT)
    polygongrid.store()

def hello_raster(subDir):
    Engine.setWorkingCatalog(workingDir+subDir)
    rc = RasterCoverage("n000302.mpr")
    res = Engine.do("aggregateraster",rc,"Avg",10,True)
    if res.connectTo(workingDir+subDir+"/avg_n000302", "map","ilwis3",IlwisObject.cmOUTPUT):
        if res.store():
            print("done!")

def hello_feature(subDir):
    Engine.setWorkingCatalog(workingDir+subDir)
    fc_soils = FeatureCoverage("aafsoils.shp")
    count = 0
    for feature in fc_soils:
        if float(feature["AREA"]) == 0.123:
            count += 1
            print(feature.geometry())
    print("contains ",count," features with an AREA equal to 0.123" )

#here you can chose which test case will be executed
if __name__ == "__main__":
    main()
