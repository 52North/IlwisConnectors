#!/usr/bin python
# -*- coding: utf-8 -*-
from _testbuffer import py_buffer_to_contiguous

import unittest as ut

from ilwisobjects import *

workingDir = "file:///C:/Users/Poku/dev/Ilwis4/testdata"
pytestDir = "/pytest"
babyDir = "/baby"
exampleDir = "/example"

class TestModule(ut.TestCase):
    def test_IssueLogger(self):
        disconnectIssueLogger()
        connectIssueLogger()
        Engine.setWorkingCatalog(workingDir)
        #TODO should return error.....
        fc = FeatureCoverage("nonexistent.mpr")
        self.assertTrue(bool(fc))
        fc = FeatureCoverage("nonexistent.file")
        self.assertTrue(bool(fc))

class TestData(ut.TestCase):
    def test_integerValues(self):
        """test the integer boundaries of the SWIG(C++) methods on Feature class"""
        pV = PyVariant(-9223372036854775808)
        self.assertEqual(int(pV), -9223372036854775808)#MIN(qlonglong)
        pV = PyVariant(-9223372036854775809)
        self.assertEqual(int(pV), -1)#overflow(MIN-1)
        pV = PyVariant(9223372036854775807)
        self.assertEqual(int(pV), 9223372036854775807)#MAX(qlonglong)
        pV = PyVariant(9223372036854775808)
        self.assertEqual(int(pV), -1)#overflow(MAX+1)
        pV = PyVariant("9223372036854775808")
        with self.assertRaises(TypeError,msg="did not overflow unsigned long long int") as e:
            int(pV)

    def test_floatValues(self):
        pV = PyVariant("9223372036854775808")
        self.assertEqual(float(pV),9223372036854775808)
        pV = PyVariant(9223372036854775808.)
        self.assertEqual(float(pV),9223372036854775808)
        pV = PyVariant(0.432)
        self.assertEqual(float(pV), 0.432)
        pV = PyVariant(23.4e-32)
        self.assertEqual(float(pV), 23.4e-32)


class TestOperation(ut.TestCase):
    def setUp(self):
        Engine.setWorkingCatalog(workingDir + pytestDir)
        self.cs = CoordinateSystem(
            "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
    def tearDown(self):
        del self.cs

    def test_gridding(self):
        polygongrid = Engine.do("gridding", self.cs, Coordinate(225358.6605, 3849480.5700), 1000.0, 1000.0, 12, 12)
        self.assertTrue(bool(polygongrid),msg="gridding result is invalid!")
        self.assertEqual(polygongrid.type(),"FeatureCoverage",msg="wrong IlwisObject type")
        self.assertRegex(polygongrid.name(),r"gridding_[0-9]*",msg="generated name should begin with gridding_ and end with its ID")
        self.assertEqual(polygongrid.featureCount(),144,msg="wrong number of polygons in gridding result!")


class TestFeature(ut.TestCase):
    def setUp(self):
        Engine.setWorkingCatalog(workingDir + pytestDir)
        self.fc = FeatureCoverage("rainfall.shp")
        self.assertTrue(self.fc, msg="FeatureCoverage(rainfall.shp) not loaded correctly!")
    def tearDown(self):
        del self.fc

    def test_FeatureCoverage(self):
        self.assertEqual(self.fc.name(), "rainfall.shp", msg="internal FeatureCoverage name wrong!")
        self.assertEqual(self.fc.featureCount(), 13, msg="feature count wrong")

        self.assertTrue(self.fc.addAttribute("sum", "value"),msg="FeatureCoverage.addAttribute failed!")
        att = self.fc.attributes()
        self.assertTupleEqual(att,('coverage_key', 'RAINFALLMPP', 'RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE', 'JULY', 'AUGUST', 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'sum'),msg="wring list of attributes!")
        self.assertEqual(len(att),18,msg="wrong number of attributes")
        it = iter(self.fc)#a HACK to loadBinaryData before newFeature is created/added to the FeatureCoverage!
        g = Geometry("POINT(5.4 6 9.0)")
        self.assertEqual(str(g),"POINT(5.4 6 9.0)","standalone Geometry(fromWKT) failed!")
        newF = self.fc.newFeature(g);
        self.assertTrue(bool(newF),msg="newFeature creation failed!")
        for c in self.fc.attributes():
            newF[c] = 12.0
            self.assertEqual(float(newF[c]),12.0,msg="new value of feature attribute not correct!")

        sum = 0
        for f in self.fc:
            sum += float(f.attribute("MAY", 0))
            f["sum"] = sum
            self.assertRegex(str(f),r"Feature\([0-9]*\)",msg="wrong feature representation")
            self.assertRegex(str(f.geometry()), r"POINT\(([0-9]+|\-1e\+308|5\.4)\s([0-9]+|\-1e\+308|[0-9]+\.[0-9]+e\+[0-9]+)\s(0|9)\)", msg="wrong geometry representation")
        self.assertEqual(sum, 298.0, msg="wrong sum over rainfall in MAY!")
        del sum

    def test_FeatureIterator(self):
        it = iter(self.fc)
        f = next(it)

        with self.assertRaises(Exception,msg="no Exception from wrong WKT") as ao:
            f.geometry().fromWKT("POINT(5.4 6 9.0 0)")

        self.assertTrue(f.geometry().fromWKT("POINT(5.4 6 9.0)"),msg="set non-standalone Geometry fromWKT")
        self.assertEqual(f.geometry().toWKT(),"POINT(5.4 6 9)", msg="unsuccessfully altered geometry")

        with self.assertRaises(IndexError,msg="no IndexError on call of wrong attribute") as ao:
            v = f["RAINFAL"]

        v = f["RAINFALL"]
        self.assertEqual(str(v),"UMSS",msg="wrong attribute value")
        with self.assertRaises(TypeError,msg="no TypeError on attempt to convert non-numerical string to integer") as ao:
            print(int(v))



class TestCoordinateSystem(ut.TestCase):
    def setUp(self):
        Engine.setWorkingCatalog(workingDir + pytestDir)
        self.fc = FeatureCoverage("rainfall.shp")
    def tearDown(self):
        del self.fc

    def test_Proj4(self):
        cs1 = CoordinateSystem("code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
        cs1.name("myProj4CSY")
        self.assertTrue(bool(cs1),msg="invalid CoordinateSystem from Proj4")
        self.fc.setCoordinateSystem(cs1)
        self.assertEqual(self.fc.coordinateSystem().name(),"myProj4CSY",msg="could not alter FeatureCoverage's CoordinateSystem")
    def test_EPSG(self):
        cs2 = CoordinateSystem("code=epsg:23035")
        self.assertTrue(bool(cs2))
        self.assertEqual(cs2.name(),r"ED50 / UTM zone 35N")

class TestRaster(ut.TestCase):
    def setUp(self):
        Engine.setWorkingCatalog(workingDir + pytestDir)

    def test_RasterCalculation(self):
        rc = RasterCoverage("n000302.mpr")
        rctif = RasterCoverage("n0.mpr")
        self.assertTrue(rc and rctif,msg="couldn't load RasterCoverages")
        self.assertEqual(rc.name(),"n000302.mpr")
        self.assertEqual(rctif.name(),"n0.mpr")
        self.assertEqual(rc.pix2value(342, 342, 0),29.0)
        self.assertEqual(rc.coord2value(Coordinate(-1871900.47, 1874237.55)),29.0)

        #print(rctif.name() + ".pix2value(342,342,0) =>", rctif.pix2value(342, 342, 0))
        #print("-----------------------------------------------")
        #aa7 = Engine.do("sin", "n000302.mpr")
        #print("sin(n000302.mpr)=>", aa7.name() + ".pix2value(342,342,0)=>", aa7.pix2value(342, 342, 0))
        #print("-----------------------------------------------")
        #aa1 = rc + rctif
        #print(rc.name(), " + ", rctif.name(), " = ", aa1.name() + ".pix2value(342,342,0)=>", aa1.pix2value(342, 342, 0))
        #aa2 = rc + 2
        #print(rc.name(), " + 2 = ", aa2.name() + ".pix2value(342,342,0)=>", aa2.pix2value(342, 342, 0))
        #aa3 = 2 + rc
        #print("2 + ", rc.name(), " = ", aa3.name() + ".pix2value(342,342,0)=>", aa3.pix2value(342, 342, 0))
        #aa4 = rc - rctif
        #print(rc.name(), " - ", rc.name(), " = ", aa4.name() + ".pix2value(342,342,0)=>", aa4.pix2value(342, 342, 0))
        #aa5 = 2 - rc #until now this is parsed as "rc - 2" :(
        #print("2 - ", rc.name(), " = ", aa5.name() + ".pix2value(342,342,0)=>", aa5.pix2value(342, 342, 0))
        #aa6 = rc - 2
        #print(rc.name(), " - 2 = ", aa6.name() + ".pix2value(342,342,0)=>", aa6.pix2value(342, 342, 0))
        #aa7 = rc / rctif
        #print(rc.name(), " / ", rc.name(), " = ", aa7.name() + ".pix2value(342,342,0)=>", aa7.pix2value(342, 342, 0))
        #aa8 = 2 / rc #until now this is parsed as "rc / 2" :(
        #print("2 / ", rc.name(), " = ", aa8.name() + ".pix2value(342,342,0)=>", aa8.pix2value(342, 342, 0))
        #aa9 = rc / 2
        #print(rc.name(), " / 2 = ", aa9.name() + ".pix2value(342,342,0)=>", aa9.pix2value(342, 342, 0))
        #aa10 = rc * rctif
        #print(rc.name(), " * ", rc.name(), " = ", aa10.name() + ".pix2value(342,342,0)=>", aa10.pix2value(342, 342, 0))
        #aa11 = 2 * rc
        #print("2 * ", rc.name(), " = ", aa11.name() + ".pix2value(342,342,0)=>", aa11.pix2value(342, 342, 0))
        #aa12 = rc * 2
        #print(rc.name(), " * 2 = ", aa12.name() + ".pix2value(342,342,0)=>", aa12.pix2value(342, 342, 0))
        #print("-----------------------------------------------")
        ##store to file
        #if aa1.connectTo(workingDir + subDir + "/aa1", "GTiff", "gdal", IlwisObject.cmOUTPUT):
        #    if aa1.store():
        #        print("successfully saved aa1.tif")
        #    else:
        #        print("could not save aa1.tif")
        #else:
        #    print("connectTo file failed!")
#
#
def pixelIterator(subDir):
    Engine.setWorkingCatalog(workingDir + subDir)
    #iterate over pixel
    rcl = RasterCoverage("small.mpl")
    if rcl:
        print()
        it = iter(rcl)
        if it:
            for i in range(175):
                print(next(it), end="|")
                if it.yChanged():
                    print()
                if it.zChanged():
                    print("====================")
        print()
        rit = iter(rcl)
        if rit:
            rit += 4
            rit2 = rit + 3
            rit3 = 13 + rit
            print(rit3.position(), "==", rit2.position(), "=>", rit3 == rit2)
            print("float(iter(small.mpl)+=1)=>", float(rit2))
            print("iter(small.mpl)[23]=>", rit[23])
            print("float(iter(small.mpl))=>", float(rit))
            print("iter(small.mpl)[23]=434")
            rit[23] = 434
            print("iter(small.mpl)[23]=>", rit[23])
            rit2 = PixelIterator(rit[Voxel(2, 2, 2)])
            print("next(iter(small.mpl)[", rit.position(), "==", int(rit), "])=>", next(rit))
            print(rit.position(), rit.xChanged(), rit.yChanged(), rit.zChanged())
            print(rit.position(), "==", rit2.position(), "=>", rit == rit2)
            print(rit.position(), "!=", rit2.position(), "=>", rit != rit2)
            print(rit.position(), ">=", rit2.position(), "=>", rit >= rit2)
            print(rit.position(), ">", rit2.position(), "=>", rit > rit2)
            print(rit.position(), "<=", rit2.position(), "=>", rit <= rit2)
            print(rit.position(), "<", rit2.position(), "=>", rit < rit2)
            print("float(iter(small.mpl)[", rit.position(), "==", int(rit), "])=>", float(rit))
        box = Box("(1 1 0,2 2 3)")
        print("PixelIterator(", rcl.name(), ",", box, ")")
        bit = PixelIterator(rcl, box)
        if bit:
            for i in range(bit.box().size().linearSize()):
                print("(", bit.position(), ")", next(bit), end="|")
                if bit.yChanged():
                    print()
                if bit.zChanged():
                    print("====================")
            print()


class TestExample(ut.TestCase):#and martins solution proposal <== example code for presentation
    def setUp(self):
        Engine.setWorkingCatalog(workingDir + exampleDir)

    def test_claudio(self):
        distribution = FeatureCoverage(workingDir + exampleDir + "/freq.mpp")
        polygongrid = Engine.do("gridding", distribution.coordinateSystem(), Coordinate(26.5, 4.5), 1, 1, 15, 13)
        self.assertRegex(polygongrid.name(),r"gridding_[0-9]*")
        polygongrid.addAttribute("maxY", "value")
        for polygon in polygongrid:
        #        polygon.setAttribute("maxY", 0)
            for point in distribution:
                if polygon.geometry().contains(point.geometry()):
                    maxval = max(int(polygon.attribute("maxY", 0)), int(point.attribute("freq_speciesY", 0)))
                    polygon.setAttribute("maxY", maxval)

        self.assertTrue(polygongrid.connectTo(workingDir + exampleDir + "/polygongrid", "polygonmap", "ilwis3", IlwisObject.cmOUTPUT))
        self.assertTrue(polygongrid.store())


class TestBaby(ut.TestCase):
    def setUp(self):
        Engine.setWorkingCatalog(workingDir + babyDir)

    def test_helloRaster(self):
        rc = RasterCoverage("n000302.mpr")
        res = Engine.do("aggregateraster", rc, "Avg", 10, True)
        self.assertTrue(res.connectTo(workingDir + babyDir + "/avg_n000302", "map", "ilwis3", IlwisObject.cmOUTPUT))
        self.assertTrue(res.store())

    def test_helloFeature(self):
        fc_soils = FeatureCoverage("aafsoils.shp")
        count = 0
        for feature in fc_soils:
            if float(feature["AREA"]) == 0.123:
                count += 1
                self.assertRegex(str(feature.geometry()),r"POLYGON\([\s\.\-\,\(\)0-9]*\)",msg="wrong WKT representation of geometry!")
        self.assertEqual(count,3,msg="wrong count value!") #print("contains ", count, " features with an AREA equal to 0.123")

#here you can chose which test case will be executed
if __name__ == "__main__":
    #main()
    ut.main(verbosity=2)


