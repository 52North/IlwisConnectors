#!/usr/bin python
# -*- coding: utf-8 -*-
try:
    from ilwisobjects import *

    workingDir = "file:///C:/Users/Poku/dev/Ilwis4/testdata"
    pytestDir = "/pytest"
    babyDir = "/baby"
    exampleDir = "/example"

    import unittest as ut

    #@ut.skip("temporarily")
    class TestGeometry(ut.TestCase):
        def test_Geometry(self):
            g = Geometry("POINT(5.4 6 9.0)")
            self.assertEqual(str(g), "POINT(5.4 6 9.0)", "standalone Geometry(fromWKT) failed!")
            self.assertEqual(g.toWKT(), "POINT(5.4 6 9.0)", "standalone Geometry(fromWKT) failed!")
            self.assertTrue(bool(g))
            g = Geometry("Pihkdjfhskdf")
            self.assertEqual(str(g), "Pihkdjfhskdf", "standalone Geometry(fromWKT) failed!")
            self.assertEqual(g.toWKT(), "Pihkdjfhskdf", "standalone Geometry(fromWKT) failed!")

        def test_Size(self):
            sz = Size(2, 4, 5)
            self.assertEqual(str(sz), "Size(2, 4, 5)")
            self.assertEqual(sz.linearSize(),2*4*5)
            sz *= 2
            self.assertEqual(str(sz), "Size(4, 8, 10)")
            sz.xsize = 3
            self.assertEqual(sz.xsize, 3)
            self.assertTrue(sz.contains(Voxel(1, 1, 1)))
            self.assertFalse(sz.contains(Voxel(5, 5, 5)))

        def test_Box(self):
            b = Box(Voxel(3, 4, 5), Voxel(4, 5, 6,))
            self.assertEqual(str(b), "POLYGON(3 4 5,4 5 6)")
            self.assertEqual(str(b.size()), "Size(2, 2, 2)")
            self.assertTrue(b.size() == Size(2, 2, 2))
            self.assertEqual(b.size().linearSize(),2*2*2)

    #@ut.skip("temporarily")
    class TestModule(ut.TestCase):
        def setUp(self):
            try:
                Engine.setWorkingCatalog(workingDir+pytestDir)
            except IlwisException:
                self.skipTest("could not set working directory!")

        def test_IssueLogger(self):
            disconnectIssueLogger()
            fc = FeatureCoverage(workingDir+pytestDir+"nonexistent.file")
            self.assertFalse(bool(fc))
            connectIssueLogger()

    #@ut.skip("temporarily")
    class TestData(ut.TestCase):
        def test_UNDEF(self):
            with self.assertRaises(AttributeError, msg="property is not read only!"):
                Const.sUNDEF = "test"
            self.assertEqual(Const.sUNDEF, "?")
            self.assertEqual(Const.shUNDEF, -32767)
            self.assertEqual(Const.iUNDEF, -2147483647)
            self.assertEqual(Const.flUNDEF, -1e38)
            self.assertEqual(Const.rUNDEF, -1e308)
            self.assertEqual(Const.i64UNDEF, -9223372036854775808)

        def test_integerValues(self):
            pv = PyVariant(-9223372036854775808)
            self.assertEqual(int(pv), -9223372036854775808)  # MIN(qlonglong)
            pv = PyVariant(-9223372036854775809)
            self.assertEqual(int(pv), -1)  # overflow(MIN-1)
            pv = PyVariant(9223372036854775807)
            self.assertEqual(int(pv), 9223372036854775807)  # MAX(qlonglong)
            pv = PyVariant(9223372036854775808)
            self.assertEqual(int(pv), -1)  # overflow(MAX+1)
            pv = PyVariant("9223372036854775808")
            with self.assertRaises(TypeError, msg="did not overflow unsigned long long int"):
                int(pv)

        def test_floatValues(self):
            pv = PyVariant("9223372036854775808")
            self.assertEqual(float(pv), 9223372036854775808)
            pv = PyVariant(9223372036854775808.)
            self.assertEqual(float(pv), 9223372036854775808)
            pv = PyVariant(0.432)
            self.assertEqual(float(pv), 0.432)
            pv = PyVariant(23.4e-32)
            self.assertEqual(float(pv), 23.4e-32)

    #@ut.skip("temporarily")
    class TestOperation(ut.TestCase):
        def setUp(self):
            try:
                Engine.setWorkingCatalog(workingDir + pytestDir)
            except IlwisException:
                self.skipTest("could not set working directory!")

            self.cs = CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")

        def tearDown(self):
            del self.cs

        def test_gridding(self):
            polygongrid = Engine.do("gridding", self.cs, Coordinate(225358.6605, 3849480.5700), 1000.0, 1000.0, 12, 12)
            self.assertTrue(bool(polygongrid), msg="gridding result is invalid!")
            self.assertEqual(polygongrid.type(), "FeatureCoverage", msg="wrong IlwisObject type")
            self.assertRegex(polygongrid.name(), r"gridding_[0-9]*",
                             msg="generated name should begin with gridding_ and end with its ID")
            self.assertEqual(polygongrid.featureCount(), 144, msg="wrong number of polygons in gridding result!")

    #@ut.skip("temporarily")
    class TestFeature(ut.TestCase):
        def setUp(self):
            try:
                Engine.setWorkingCatalog(workingDir + pytestDir)
            except IlwisException:
                self.skipTest("could not set working directory!")

            self.fc = FeatureCoverage("rainfall.shp")
            self.assertTrue(self.fc, msg="FeatureCoverage(rainfall.shp) not loaded correctly!")

        def tearDown(self):
            del self.fc

        #@ut.skip("temporarily")
        def test_FeatureCoverage(self):
            self.assertEqual(self.fc.name(), "rainfall.shp", msg="internal FeatureCoverage name wrong!")
            self.assertEqual(self.fc.featureCount(), 13, msg="feature count wrong")

            self.assertTrue(self.fc.addAttribute("sum", "value"), msg="FeatureCoverage.addAttribute failed!")
            att = self.fc.attributes()
            self.assertTupleEqual(att, (
                'coverage_key', 'RAINFALLMPP', 'RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE',
                'JULY', 'AUGUST', 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'sum'
            ), msg="wring list of attributes!")
            self.assertEqual(len(att), 18, msg="wrong number of attributes")
            iter(self.fc)  # a HACK to loadBinaryData before newfeatureeature is created/added to the FeatureCoverage!
            g = Geometry("POINT(5.4 6 9.0)")
            newfeature = self.fc.newFeature(g)
            self.assertTrue(bool(newfeature), msg="newfeature creation failed!")
            for c in self.fc.attributes():
                newfeature[c] = 12.0
                self.assertEqual(float(newfeature[c]), 12.0, msg="new value of feature attribute not correct!")

            self.assertEqual(self.fc.featureCount(), 14, msg="new feature count wrong")

        #@ut.skip("temporarily")
        def test_Feature(self):
            it = iter(self.fc)
            f = next(it)

            with self.assertRaises(Exception, msg="no Exception from wrong WKT"):
                f.geometry().fromWKT("POINT(5.4 6 9.0 0)")

            self.assertTrue(f.geometry().fromWKT("POINT(5.4 6 9.0)"), msg="set non-standalone Geometry fromWKT")
            self.assertEqual(f.geometry().toWKT(), "POINT(5.4 6 9)", msg="unsuccessfully altered geometry")

            with self.assertRaises(IndexError, msg="no IndexError on call of wrong attribute"):
                v = f["RAINFAL"]

            v = f["RAINFALL"]
            self.assertEqual(str(v), "UMSS", msg="wrong attribute value")
            with self.assertRaises(TypeError, msg="no TypeError on attempt to convert non-numerical string to int"):
                print(int(v))

        def test_FeatureIterator(self):
            summ = 0
            for f in self.fc:
                summ += float(f.attribute("MAY", 0))
                f["sum"] = summ
                self.assertRegex(str(f), r"Feature\([0-9]*\)", msg="wrong feature representation")
                self.assertRegex(str(f.geometry()),
                                 r"POINT\(([0-9]+|\-1e\+308|5\.4)\s([0-9]+|\-1e\+308|[0-9]+\.[0-9]+e\+[0-9]+)\s(0|9)\)",
                                 msg="wrong geometry representation")
            self.assertEqual(summ, 286.0, msg="wrong sum over rainfall in MAY!")
            del summ
            it = iter(self.fc)
            self.assertTrue(bool(it))
            self.assertEqual(str(it),"FeatureIterator for rainfall.shp")
            it2 = it + 2
            self.assertTrue(it != it2)
            self.assertFalse(it == it2)
            it3 = it2 - 2
            self.assertTrue(it == it3)
            f = it3.current()
            self.assertTrue(str(f),"Feature(0)")
            f = next(it3)
            self.assertTrue(str(f),"Feature(0)")


    @ut.skip("temporarily")
    class TestCoordinateSystem(ut.TestCase):
        def setUp(self):
            try:
                Engine.setWorkingCatalog(workingDir + pytestDir)
            except IlwisException:
                self.skipTest("could not set working directory!")
            self.fc = FeatureCoverage("rainfall.shp")

        def tearDown(self):
            del self.fc

        def test_Proj4(self):
            cs1 = CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
            cs1.name("myProj4CSY")
            self.assertTrue(bool(cs1), msg="invalid CoordinateSystem from Proj4")
            self.fc.setCoordinateSystem(cs1)
            self.assertEqual(self.fc.coordinateSystem().name(), "myProj4CSY",
                             msg="could not alter FeatureCoverage's CoordinateSystem")

        def test_EPSG(self):
            cs2 = CoordinateSystem("code=epsg:23035")
            self.assertTrue(bool(cs2))
            self.assertEqual(cs2.name(), r"ED50 / UTM zone 35N")

    #@ut.skip("temporarily")
    class TestRaster(ut.TestCase):
        def setUp(self):
            try:
                Engine.setWorkingCatalog(workingDir + pytestDir)
            except IlwisException:
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_RasterCalculation(self):
            rc = RasterCoverage("n000302.mpr")
            rctif = RasterCoverage("n0.mpr")
            self.assertTrue(rc and rctif, msg="couldn't load RasterCoverages")
            self.assertEqual(rc.name(), "n000302.mpr")
            self.assertEqual(rctif.name(), "n0.mpr")
            self.assertEqual(rc.pix2value(342, 342, 0), 29.0)
            self.assertEqual(rc.coord2value(Coordinate(-1871900.47, 1874237.55)), 29.0)
            self.assertEqual(rctif.pix2value(342, 342, 0), 29.0)

            aa7 = Engine.do("sin", rc)
            self.assertAlmostEqual(aa7.pix2value(342, 342, 0), -0.663, 3)

            aa1 = rc + rctif
            self.assertEqual(aa1.pix2value(342, 342, 0), 29.0 + 29.0)
            aa2 = rc + 2
            self.assertEqual(aa2.pix2value(342, 342, 0), 29.0 + 2)
            aa3 = 2 + rc
            self.assertEqual(aa3.pix2value(342, 342, 0), 2 + 29.0)
            aa4 = rc - rctif
            self.assertEqual(aa4.pix2value(342, 342, 0), 29.0 - 29.0)
            #TODO fix parsing of numeric - raster (don't switch arguments)
            aa5 = 2 - rc  # until now this is parsed as "rc - 2" :(
            #self.assertEqual(aa5.pix2value(342, 342, 0),2-29.0)
            aa6 = rc - 2
            self.assertEqual(aa6.pix2value(342, 342, 0), 29.0 - 2)
            aa7 = rc / rctif
            self.assertEqual(aa7.pix2value(342, 342, 0), 29.0 / 29.0)
            #TODO fix parsing of numeric / raster (don't switch arguments)
            aa8 = 2 / rc  # until now this is parsed as "rc / 2" :(
            #self.assertEqual(aa8.pix2value(342, 342, 0),2/29.0)
            aa9 = rc / 2
            self.assertAlmostEqual(aa9.pix2value(342, 342, 0), 29.0 / 2, 1)
            aa10 = rc * rctif
            self.assertEqual(aa10.pix2value(342, 342, 0), 29.0 * 29.0)
            aa11 = 2 * rc
            self.assertEqual(aa11.pix2value(342, 342, 0), 2 * 29.0)
            aa12 = rc * 2
            self.assertEqual(aa12.pix2value(342, 342, 0), 29.0 * 2)

            self.assertTrue(aa1.connectTo(workingDir + pytestDir + "/aa1", "GTiff", "gdal", IlwisObject.cmOUTPUT),
                            msg="connectTo file failed!")
            self.assertTrue(aa1.store(), msg="could not save aa1.tif")

        def test_PixelIterator(self):
            rcl = RasterCoverage("small.mpl")
            self.assertTrue(bool(rcl), msg="couldn't load small.mpl")
            self.assertEqual(rcl.name(), "small.mpl")
            small = [
                3.0, 80.0, 80.0, 80.0, 3.0,
                15.0, 22.0, 22.0, 130.0, 100.0,
                15.0, 132.0, 35.0, 21.0, 100.0,
                15.0, 34.0, 67.0, 179.0, 100.0,
                3.0, 120.0, 120.0, 120.0, 3.0,

                2.0, 99.0, 99.0, 99.0, 2.0,
                20.0, 32.0, 73.0, 44.0, 55.0,
                20.0, 78.0, 24.0, 44.0, 55.0,
                20.0, 34.0, 132.0, 86.0, 55.0,
                2.0, 145.0, 145.0, 145.0, 2.0,

                1.0, 30.0, 30.0, 30.0, 1.0,
                70.0, 60.0, 15.0, 32.0, 150.0,
                70.0, 12.0, 10.0, 101.0, 150.0,
                70.0, 69.0, 170.0, 99.0, 150.0,
                1.0, 120.0, 120.0, 120.0, 1.0,

                34.0, 34.0, 123.0, 123.0, 45.0,
                34.0, 174.0, 174.0, -1e+308, 45.0,
                78.0, 78.0, 174.0, -1e+308, 29.0,
                78.0, 78.0, 1.0, 156.0, 29.0,
                78.0, 78.0, 7.0, 7.0, 29.0,

                32.0, 150.0, 150.0, 22.0, 22.0,
                32.0, 77.0, 88.0, 131.0, 22.0,
                117.0, 117.0, 88.0, 5.0, 5.0,
                45.0, 2.0, 88.0, 2.0, 101.0,
                45.0, 45.0, 150.0, 101.0, 101.0,

                90.0, 6.0, 135.0, 90.0, 120.0,
                90.0, 2.0, 117.0, 4.0, 90.0,
                45.0, 45.0, 135.0, 45.0, 45.0,
                90.0, 3.0, 78.0, 5.0, 90.0,
                7.0, 90.0, 135.0, 90.0, 9.0,

                179.0, 110.0, 179.0, 67.0, 110.0,
                90.0, 179.0, 23.0, 90.0, 110.0,
                110.0, 34.0, 67.0, 110.0, 78.0,
                179.0, 67.0, 90.0, 110.0, 67.0,
                90.0, 179.0, 77.0, 90.0, 88.0,
            ]
            i = 0
            for v in rcl:
                self.assertEqual(v, small[i])
                i += 1

            it = iter(rcl)
            self.assertEqual(str(it),"PixelIterator for small.mpl at position pixel(0,0,0)",
                             msg="wrong string representation for PixelIterator")
            self.assertTrue(bool(it), msg="couldn't create PixelIterator from small.mpl")
            lin = rcl.size().linearSize()
            self.assertEqual(lin, 175, msg="wrong linear size of small.mpl")
            for i in range(lin):
                if it.yChanged():
                    self.assertTrue(i % 5 == 0, msg="yChanged not only every 5th step (i=" + str(i) + ")")
                else:
                    self.assertFalse((i % 5 == 0) and (i != 0), msg="yChanged not only every 2nd step (i="+str(i)+")")
                if it.zChanged():
                    self.assertTrue(i % 25 == 0, msg="zChanged not only every 25th step (i=" + str(i) + ")")
                else:
                    self.assertFalse((i % 25 == 0) and (i != 0), msg="zChanged not only every 2nd step (i="+str(i)+")")
                self.assertEqual(next(it), small[i])

            rit = iter(rcl)
            self.assertTrue(bool(rit))
            self.assertEqual(str(rit.position()), "pixel(0,0,0)")
            rit += 4
            self.assertEqual(str(rit.position()), "pixel(4,0,0)")
            rit2 = rit + 3
            self.assertEqual(str(rit2.position()), "pixel(2,1,0)")
            rit3 = 13 + rit
            self.assertEqual(str(rit3.position()), "pixel(2,3,0)")
            self.assertEqual(str(rit.position()), "pixel(4,0,0)")
            self.assertTrue(rit.contains(Voxel(1, 1, 1)))
            self.assertFalse(rit.contains(Voxel(5, 1, 1)))
            self.assertFalse(rit3 == rit2)
            self.assertEqual(float(rit2), 22.0)
            self.assertEqual(rit[23], 120.0)
            self.assertEqual(float(rit), 120.0)
            rit[23] = 434
            self.assertEqual(rit[23], 434)
            rit2 = PixelIterator(rit[Voxel(2, 2, 2)])
            self.assertEqual(str(rit.position()), "pixel(2,2,2)")
            self.assertEqual(int(rit), 62)
            self.assertEqual(next(rit), 10.0)
            self.assertTrue(rit.xChanged())
            self.assertFalse(rit.yChanged())
            self.assertFalse(rit.zChanged())
            self.assertEqual(str(rit.position()), "pixel(3,2,2)")
            self.assertEqual(str(rit2.position()), "pixel(2,2,2)")
            self.assertFalse(rit == rit2)
            self.assertTrue(rit != rit2)
            self.assertTrue(rit >= rit2)
            self.assertTrue(rit > rit2)
            self.assertFalse(rit <= rit2)
            self.assertFalse(rit < rit2)
            self.assertEqual(int(rit), 63)
            self.assertEqual(float(rit), 101.0)
            box = Box("(1 1 0,2 2 3)")
            self.assertEqual(str(box), "POLYGON(1 1 0,2 2 3)")
            bit = PixelIterator(rcl, box)
            self.assertTrue(bool(bit))
            boxed_small = [
                ("pixel(1,1,0)", 22.0), ("pixel(2,1,0)", 22.0),
                ("pixel(1,2,0)", 132.0), ("pixel(2,2,0)", 35.0),
                ("pixel(1,1,1)", 32.0), ("pixel(2,1,1)", 73.0),
                ("pixel(1,2,1)", 78.0), ("pixel(2,2,1)", 24.0),
                ("pixel(1,1,2)", 60.0), ("pixel(2,1,2)", 15.0),
                ("pixel(1,2,2)", 12.0), ("pixel(2,2,2)", 10.0),
                ("pixel(1,1,3)", 174.0), ("pixel(2,1,3)", 174.0),
                ("pixel(1,2,3)", 78.0), ("pixel(2,2,3)", 174.0),
            ]
            self.assertTrue(bit.contains(Voxel(1, 1, 1)))
            self.assertFalse(bit.contains(Voxel(0, 0, 0)))
            lin = bit.box().size().linearSize()
            self.assertEqual(lin, 16)
            for i in range(lin):
                self.assertEqual(str(bit.position()), boxed_small[i][0])
                if bit.yChanged():
                    self.assertTrue(i % 2 == 0, msg="yChanged not only every 2nd step (i=" + str(i) + ")")
                else:
                    self.assertFalse((i % 2 == 0) and (i != 0), msg="yChanged not only every 2nd step (i="+str(i)+")")
                if bit.zChanged():
                    self.assertTrue(i % 4 == 0, msg="zChanged not only every 4th step (i=" + str(i) + ")")
                else:
                    self.assertFalse((i % 4 == 0) and (i != 0), msg="zChanged not only every 4th step (i="+str(i)+")")
                self.assertEqual(next(bit), boxed_small[i][1])

    #@ut.skip("temporarily")
    class TestExample(ut.TestCase):  # and martins solution proposal <== example code for presentation
        def setUp(self):
            try:
                Engine.setWorkingCatalog(workingDir + exampleDir)
            except IlwisException:
                self.skipTest("could not set working directory!")

        def test_claudio(self):
            distribution = FeatureCoverage(workingDir + exampleDir + "/freq.mpp")
            polygongrid = Engine.do("gridding", distribution.coordinateSystem(), Coordinate(26.5, 4.5), 1, 1, 15, 13)
            self.assertRegex(polygongrid.name(), r"gridding_[0-9]*")
            polygongrid.addAttribute("maxY", "value")
            for polygon in polygongrid:
            #        polygon.setAttribute("maxY", 0)
                for point in distribution:
                    if polygon.geometry().contains(point.geometry()):
                        maxval = max(int(polygon.attribute("maxY", 0)), int(point.attribute("freq_speciesY", 0)))
                        polygon.setAttribute("maxY", maxval)

            self.assertTrue(polygongrid.connectTo(workingDir + exampleDir + "/polygongrid", "polygonmap", "ilwis3",
                                                  IlwisObject.cmOUTPUT))
            self.assertTrue(polygongrid.store())

    #@ut.skip("temporarily")
    class TestBaby(ut.TestCase):
        def setUp(self):
            try:
                Engine.setWorkingCatalog(workingDir + babyDir)
            except IlwisException:
                self.skipTest("could not set working directory!")

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
                    self.assertRegex(str(feature.geometry()), r"POLYGON\([\s\.\-\,\(\)0-9]*\)",
                                     msg="wrong WKT representation of geometry!")
            self.assertEqual(count, 3,
                             msg="wrong count value!")  # print("contains",count,"features with an AREA equal to 0.123")

    #here you can chose which test case will be executed
    if __name__ == "__main__":
        ut.main(verbosity=2)

except ImportError as e:
    print(e)
