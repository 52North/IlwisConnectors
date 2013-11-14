#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ilwisobjects import *

def main():
    Engine.setWorkingCatalog("file:///C:/Users/Poku/dev/Ilwis4/testdata/shape")

    localcoordsystem = CoordinateSystem("code=epsg:23035")
    polygongrid = Engine.do("gridding("+localcoordsystem+",10039939, 2399393, 10045997, 2405000, 1000)")

    #muteIssueLogger()
    print("-----------------------------------------------")
    #FeatureCoverage
    fc = FeatureCoverage("file:///C:/Users/Poku/dev/Ilwis4/testdata/shape/rainfall.shp")
    if fc:
        print("successfully loaded", fc)
        print(fc ,"contains:",fc.featureCount(),"Features")

        print("-----------------------------------------------")
        #adding new attribute to coverage
        if fc.addAttribute("highest","value"):
            print("value attribute 'highest' was added to",fc)
        else:
            print("couln't add value attribute 'highest' to",fc)
        print("-----------------------------------------------")
        #iterating over features
        sum = 0
        for f in fc:
            sum += int(f.attribute("MAY"))
            f.attribute("highest",PyVariant(sum))
            print(f, ":", f.attribute("coverage_key"), ",", f.attribute("MAY"), ",", f.attribute("RAINFALL"), ",", f.attribute("highest"));
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

    print("-----------------------------------------------")
    print("CoordinateSystems")
    cs1 = CoordinateSystem("code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
    if cs1:
        print(cs1)
    else:
        print("Coudn't create CoordinateSystem from proy4 definition!")
    cs2 = CoordinateSystem("code=epsg:23035")
    if cs2:
        print(cs2)
    else:
        print("Coudn't create CoordinateSystem from EPSG code!")
    print("-----------------------------------------------")
    Engine.setWorkingCatalog("file:///C:/Users/Poku/dev/Ilwis4/testdata")
    rc = RasterCoverage("file:///C:/Users/Poku/dev/Ilwis4/testdata/n000302.mpr")
#    rc.connectTo()
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
    ilwisengine = Engine()
#    #create a feature coverage
    distribution = FeatureCoverage()#ilwisengine.features()
#    #link it to a local shape file with species distribution. the attributes will contain an attribute 'distribution'.
    distribution.connectTo("file://d:/somepath/species.shp");
#    #create a coordinate system; we could also use the csy of the distribution map but lets create (for interface sake) a coordinate system
    localcoordsystem = CoordinateSystem("code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
#    #a variant could be
    localcoordsystem = ilwisengine.coordinatesystem("code=epsg:23035")
#    #setting the bounds
#    localcoordsystem.bounds(1003700, 239900, 1004600, 2409000)
#    # create a polygon grid
    polygongrid = ilwisengine.do("gridding", localcoordsystem,10039939, 2399393, 10045997, 2405000, 1000)
#    #add an attribute for the highest distribution ; name plus domain as parameters. Others could include ranges and such. Keep it simple here
    polygongrid.addAttribute("highest","numeric")
#    #small trivial algorithm for detecting the highest point attribute per polygon
    for polygon in polygongrid:
        for point in distribution:
#            if polygon.contains(distribution.coordinatesystem(), point)
#                maxval = max(polygon.attribute("highest"), point.attribute("distribution"))
                polygon.attribute("highest", maxval)

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

def raul_example():
    centroids = FeatureCoverage("https://www.geobabble.org/phenoregions/centroids/seeds.out.phendump.2000-2011.500.final")
    pheno = RasterCoverage("https://www.geobabble.org/phenoregions/gis/phendump.2000-2011.500.2011.img")
    #kmeans returns a dictionary for raster classification
    groups = centroids.do("kmeans",15)#groups = (dict){(x1,y1,z1)=[1,4,3],(x2,y2,z2)=[2,5,6],(x3,y3,z3)=[..],..}
    #reclassification based on kmeanified groups
    for i in range(pheno.size().x()):
        for j in range(pheno.size().y()):
            k = 0
            for kmean in groups:
                try:
                    #pixel belongs to kmean if index was found otherwise ValueError occurs
                    groups[kmean].index(pheno.value(i,j,-1))
                    pheno.setValue(i,j,-1,k)
                    break
                except ValueError: pass
    regions = pheno.do("vectorize")

    #merge small regions into dominant neighbor
    for region in regions:
        if region.area() < 5000:
            cov = regions.clone();
            cov.clear()
            regionclone = region.clone()
            cov.addFeature(regionclone)
            buffer = cov.dov("buffer")
            intersects = regions.do("coveredby",buffer)
            intersects.removeFeature(regionclone)
            max = 0
            for f in intersects:
                if f.area() > max:
                    max = f.area()
                    dominant_neighbor = f
            Engine.do("merge",region,dominat_neighbor)

def raul_martin_solution():
    #simplistic way; takes a long time
    import ilwisobjects

    centroids = ilwisobjects.featurecoverage("ftp://somepath/regions_centroids.shp")
    observations = ilwisobjects.featurecoverage("file://d:/data/observ_ca.xml")
    kmeansvoronoi = centroids.do("kmeanscluster", 15)
    kmeansvoronoi.addAttribute("leafingday_speciesX", "time")
    kmeansvoronoi.addAttribute("numberOfObservation", "numeric")

    for point in observations:
        for area in kmeansvoronois:
            if area.contains(observations.coordinatesystem(), point):
                kmeansvoronoi.attribute("numberOfObservation",kmeansvoronoi.attribute("numberOfObservation") + 1)


    #####################################################################
    #simplistic way; probably faster

    import ilwisobjects

    areaRaster = ilwisobjects.featurecoverage("ftp://somepath/regions_areas.tif")
    simplifiedRaster = areaRaster.do("rankorderfilter", "median", 10)
    simplifiedPolygon = simplifiedRaster.do("rasterize",4)
    centroids = simplifiedPolygon.do("centroids")

    observations = ilwisobjects.featurecoverage("file://d:/data/observ_ca.xml")

    kmeansvoronoi = centroids.do("kmeanscluster", 15)

    kmeansvoronoi.addAttribute("leafingday_speciesX", "time")
    kmeansvoronoi.addAttribute("numberOfObservation", "numeric")

    for point in observations:
        for area in kmeansvoronois:
                if area.contains(observations.coordinatesystem(), point):
                    kmeansvoronoi.attribute("numberOfObservation", kmeansvoronoi.attribute("numberOfObservation") + 1)

if __name__ == "__main__":
    main()
