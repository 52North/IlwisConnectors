#!/usr/bin python
# -*- coding: utf-8 -*-
from ilwisobjects import CoordinateSystem

try:
    from ilwisobjects import *

    workingDir = "file:///D:/Profiles/KiesslingHG/ILWIS/testdata"
    pytestDir = "/pytest"
    babyDir = "/baby"
    exampleDir = "/example"
    worldDir = "/world"

    import unittest as ut

    #@ut.skip("temporarily")
    class TestTable(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+pytestDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        def test_AttributeTable(self):
            fc = FeatureCoverage("rainfall.shp")
            t = fc.attributeTable()
            self.assertRegex(t.name(), "rainfall.shp")
            self.assertEqual(
                ('RAINFALLMPP', 'RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE', 'JULY', 'AUGUST',
                 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'feature_id', 'coverage_key'),
                t.columns()
            )
            # TODO bug fix in GdalLoader
            # self.assertEqual((48, 46, 86, 89, 44, 40, 44, 85, 89, 0, 0, 0, 0), t.column("OCTOBER"))

        def test_StandaloneTable(self):
            t = Table("rainfall.tbt")
            self.assertEqual("rainfall.tbt", t.name())
            self.assertEqual(12, t.recordCount())
            self.assertEqual(14, t.columnCount())
            t.addColumn("newColumn", "value")
            self.assertEqual(15, t.columnCount())
            self.assertEqual(
                ('january', 'february', 'march', 'april', 'may', 'june', 'july', 'august', 'september', 'october', 'november',
                 'december', 'newcol', 'ident', 'newColumn'),
                t.columns()
            )
            self.assertEqual((4, 5, 6), t.select("march < 100 AND march != 87"))
            self.assertEqual((9, 10, 11), t.select("march == "+str(Const.rUNDEF) ))
            self.assertEqual(13, t.columnIndex("ident"))
            self.assertEqual(81, int(t.cell("march", 4)))
            self.assertEqual(2, t.columnIndex("march"))
            self.assertEqual([81, 76, 79], [int(t.cell(2, rec)) for rec in t.select("march < 100 AND march != 87")])
            self.assertEqual(Const.rUNDEF, float(t.cell("newColumn", 0)))
            t.setCell("newColumn", 0, 32)
            self.assertEqual(32, int(t.cell("newColumn", 0)))
            disconnectIssueLogger()
            t.setCell("newColumn", 0, "text")
            connectIssueLogger()
            self.assertEqual(Const.rUNDEF, float(t.cell("newColumn", 0)))
            self.assertEqual((87, 87, 160, 150, 81, 76, 79, 155, 160, -1e+308, -1e+308, -1e+308), t.column("march"))
            self.assertEqual((87, 87, 160, 150, 81, 76, 79, 155, 160, -1e+308, -1e+308, -1e+308), t.column(2))
            self.assertEqual((175, 165, 160, 78, 54, 35, 16, 4, 20, 86, 173, 181, 340, 2, -1e+308), t.record(2))

    #@ut.skip("temporarily")
    class TestGeometry(ut.TestCase):
        def setUp(self):
            self.csy = CoordinateSystem("code=epsg:5464")

        def tearDown(self):
            del self.csy

        def test_Geometry(self):
            g = Geometry("POINT(5.4 6 9.0)", self.csy)
            self.assertEqual(str(g), "POINT (5.4000000000000004 6.0000000000000000)",
                             "standalone Geometry(fromWKT) failed!")
            self.assertEqual(g.toWKT(), "POINT (5.4000000000000004 6.0000000000000000)",
                             "standalone Geometry(fromWKT) failed!")
            self.assertTrue(bool(g))
            self.assertEqual(g.ilwisType(), 1)
            self.assertEqual(g.coordinateSystem().name(), "Sibun Gorge 1922")

        def test_Transform(self):
            source = CoordinateSystem("code=epsg:3857")
            g = Geometry("POINT(766489.647 6840642.671)", source)
            self.assertEqual("POLYGON(766490 6.84064e+06,766490 6.84064e+06)",
                             str(g.envelope()))
            self.assertEqual("POINT (766489.6469999999972060 6840642.6710000000894070)", str(g), msg="weird toWKT from BOOST")
            target = CoordinateSystem("code=epsg:3329")
            g1 = g.transform(target)
            self.assertEqual("POINT (4945949.0809892630204558 5819418.3890127958729863)", g1.toWKT())
            #g = Geometry("POINT(6.5 52.1)", CoordinateSystem("code=proj4:+proj=longlat +ellps=WGS84 +datum=WGS84"))
            #g1 = g.transform(CoordinateSystem("code=epsg:3329"))
            #self.assertEqual("POINT(4.94595e+006 5.81942e+006)", g1.toWKT())

        def test_Envelope(self):
            g = Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
            e = g.envelope()
            self.assertEqual("POLYGON(1 1,10 10)", str(e))
            g = Geometry("POINT(1 1 1)", self.csy)
            e = g.envelope()
            self.assertEqual("POLYGON(1 1,1 1)", str(e),
                             msg="z's are always 0, since boost can only manage 2D geometries until now")

        def test_BadWKT(self):
            with self.assertRaises(SyntaxError):
                g = Geometry("Pihkdjfhskdf", self.csy)
                self.assertFalse(bool(g))
                self.assertTrue(g.fromWKT("POINT(5 5)"))
                self.assertTrue(bool(g))
                self.assertFalse(g.fromWKT("fdsfsds"))
                self.assertFalse(bool(g))

        def test_Contains(self):
            p = Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
            self.assertEqual(str(p), "POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, 1.0000000000000000 1.0000000000000000))")
            self.assertTrue(bool(p))
            pin = Geometry("POINT(5 5)",self.csy)
            self.assertEqual(str(pin), "POINT (5.0000000000000000 5.0000000000000000)")
            self.assertTrue(bool(pin))
            self.assertTrue(p.contains(pin))
            self.assertTrue(pin.within(p))

    #@ut.skip("temporarily")
    class TestUtil(ut.TestCase):
        #@ut.skip("temporarily")
        def test_Size(self):
            sz = Size(2, 4, 5)
            self.assertEqual(str(sz), "Size(2, 4, 5)")
            self.assertEqual(sz.linearSize(), 2*4*5)
            sz *= 2
            self.assertEqual(str(sz), "Size(4, 8, 10)")
            sz.xsize = 3
            self.assertEqual(sz.xsize, 3)
            self.assertTrue(Pixel(1, 1, 1) in sz)
            self.assertFalse(Pixel(5, 5, 5) in sz)

        ##@ut.skip("temporarily")
        def test_BoxEnvelope(self):
            b = Box(Pixel(3, 4, 5), Pixel(4, 5, 6,))
            self.assertEqual(str(b), "POLYGON(3 4 5,4 5 6)")
            self.assertEqual(str(b.minCorner()), "pixel(3,4,5)")
            b.minCorner().x = 39
            self.assertEqual(str(b.minCorner()), "pixel(3,4,5)")
            self.assertEqual(str(b.maxCorner()), "pixel(4,5,6)")
            self.assertEqual(str(b.size()), "Size(2, 2, 2)")
            self.assertTrue(b.size() == Size(2, 2, 2))
            self.assertEqual(b.size().linearSize(),2*2*2)
            b = Envelope(Coordinate(3.6111119, 4.7, 5.9), Coordinate(4.7, 5.8, 6.9))
            self.assertEqual(str(b), "POLYGON(3.61111 4.7 5.9,4.7 5.8 6.9)")
            self.assertEqual(str(b.size()), "Size(2, 2, 2)")
            b = Envelope("POLYGON(3.6111119 4.7 5.9,4.7 5.8 6.9)")
            self.assertEqual(str(b), "POLYGON(3.61111 4.7 5.9,4.7 5.8 6.9)")
            self.assertEqual(str(b.size()), "Size(2, 2, 2)")
            b = Box(Pixel(3, 4, 5), Pixel(4, 5, 6,))
            self.assertEqual(str(b), "POLYGON(3 4 5,4 5 6)")
            b = Envelope(Coordinate(3, 4, 5), Coordinate(4, 5, 6,))
            self.assertEqual(str(b), "POLYGON(3 4 5,4 5 6)")
            e = Envelope(b.size())
            self.assertEqual(str(e), "POLYGON(0 0 0,1 1 1)")
            self.assertTrue(e.contains(Coordinate(.5, .5, .5)))
            self.assertTrue(e.contains(e))

        def test_PixelCoordinate(self):
            p = Pixel(4, 5)
            self.assertEqual(str(p), "pixel(4,5)")
            v = Pixel(4, 5, 6)
            self.assertEqual(str(v), "pixel(4,5,6)")
            self.assertEqual(v.x, 4)
            v.x = 32
            self.assertEqual(v.x, 32)
            v.y = 32
            self.assertEqual(v.y, 32)
            v.z = 32
            self.assertEqual(v.z, 32)
            c = Coordinate(-1871900.47, 1874237.55)
            self.assertEqual(str(c), "coordinate(-1871900.470000,1874237.550000)")
            c = Coordinate(-1871900.47, 1874237.55, 2)
            self.assertEqual(str(c), "coordinate(-1871900.470000,1874237.550000,2.000000)")
            self.assertEqual(c.x, -1871900.47)
            c.x = 4.323
            self.assertEqual(c.x, 4.323)


    #@ut.skip("temporarily")
    class TestModule(ut.TestCase):
        def test_Exceptions(self):
            with self.assertRaises(IlwisException):
                raise IlwisException("test exception throwing")
            with self.assertRaises(InvalidObjectException):
                raise InvalidObjectException("test exception throwing")
            try:
                raise IlwisException("test exception throwing")
            except IlwisException as ex:
                self.assertEqual(str(ex), "test exception throwing")

        def test_IssueLogger(self):
            disconnectIssueLogger()
            fc = FeatureCoverage(workingDir+pytestDir+"/noneexistentDir/nonexistent.file")
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
    class TestEngine(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+pytestDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

            self.cs = CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")

        def tearDown(self):
            del self.cs

        #@ut.skip("temporarily")
        def test_Operations(self):
            e = Engine()
            ops = e.operations()
            oper = ('acos', 'asin', 'atan', 'assignment', 'binarylogicalraster', 'binarymathraster',
                    'binarymathfeatures', 'binarymathtable', 'ceil', 'coord2pixel', 'cos', 'cosh', 'floor',
                    'coordinate', 'pixel', 'rastersize', 'iff', 'log10', 'mastergeoreference', 'ln', 'pixel2coord',
                    'rastervalue', 'resample', 'selection', 'selection', 'selection', 'setvaluerange', 'sgn', 'sin',
                    'sinh', 'sqrt', 'stringfind', 'stringsub', 'stringreplace', 'tan', 'text2output', 'gridding',
                    'script', 'aggregateraster', 'areanumbering', 'cross', 'linearstretch', 'linearrasterfilter')
            self.assertTupleEqual(oper, ops)

        def test_Gridding(self):
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
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+pytestDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

            self.fc = FeatureCoverage("rainfall.shp")
            self.assertTrue(self.fc, msg="FeatureCoverage(rainfall.shp) not loaded correctly!")

        def tearDown(self):
            del self.fc

        ##@ut.skip("temporarily")
        def test_FeatureCoverage(self):
            self.assertEqual(self.fc.name(), "rainfall.shp", msg="internal FeatureCoverage name wrong!")
            self.assertEqual(self.fc.featureCount(), 13, msg="feature count wrong")

            self.assertTrue(self.fc.addAttribute("sum", "value"), msg="FeatureCoverage.addAttribute failed!")
            att = self.fc.attributes()
            self.assertTupleEqual(att, (
                'RAINFALLMPP', 'RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE', 'JULY',
                'AUGUST', 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'coverage_key', 'sum'
            ), msg="wrong list of attributes!")
            self.assertEqual(len(att), 18, msg="wrong number of attributes")
            g = Geometry("POINT(5.4 6 9.0)", self.fc.coordinateSystem())
            newfeature = self.fc.newFeature(g)
            self.assertTrue(bool(newfeature), msg="newfeature creation failed!")
            for c in self.fc.attributes():
                newfeature[c] = 12.0
                self.assertEqual(float(newfeature[c]), 12.0, msg="new value of feature attribute not correct!")

            self.assertEqual(self.fc.featureCount(), 14, msg="new feature count wrong")

        ##@ut.skip("temporarily")
        def test_Feature(self):
            it = iter(self.fc)
            f = next(it)
            self.assertTrue(f.geometry().fromWKT("POINT(5.4 6 9.0)"), msg="set non-standalone Geometry fromWKT")
            self.assertEqual(f.geometry().toWKT(), "POINT (5.4000000000000004 6.0000000000000000)", msg="unsuccessfully altered geometry")
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
                                 r"POINT\s\(([0-9\.\-]+|\-1e\+308|5\.4)\s([0-9\.\-]+|\-1e\+308|[0-9]+\.[0-9]+e\+[0-9]+)\)",
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
            self.assertTrue(str(f), "Feature(0)")
            f = next(it3)
            self.assertTrue(str(f), "Feature(0)")
            f = next(it3)
            self.assertTrue(str(f), "Feature(1)")


    #@ut.skip("temporarily")
    class TestCoordinateSystem(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+pytestDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        def test_FromFile(self):
            csy = CoordinateSystem(workingDir+pytestDir+"/Cochabamba.csy")
            self.assertEqual("Cochabamba.csy", csy.name())
            fc = FeatureCoverage("Rainfall.mpp")
            rainCsy = fc.coordinateSystem()
            self.assertEqual(csy.ilwisID(), rainCsy.ilwisID())
            self.assertEqual(str(csy.envelope()), str(rainCsy.envelope()))

        def test_Proj4(self):
            cs1 = CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
            cs1.name("myProj4CSY")
            self.assertTrue(bool(cs1), msg="invalid CoordinateSystem from Proj4")
            fc = FeatureCoverage(workingDir+pytestDir+"/rainfall.shp")
            fc.setCoordinateSystem(cs1)
            self.assertEqual(fc.coordinateSystem().name(), "myProj4CSY",
                             msg="could not alter FeatureCoverage's CoordinateSystem")

        def test_EPSG(self):
            cs2 = CoordinateSystem("code=epsg:23035")
            self.assertTrue(bool(cs2))
            self.assertEqual(cs2.name(), r"ED50 / UTM zone 35N")

        def test_IsEqual(self):
            cs1 = CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
            cs2 = CoordinateSystem("code=epsg:23035")
            self.assertTrue(cs1 == cs2)
            self.assertFalse(cs1 != cs2)

    #@ut.skip("temporarily")
    class TestRaster(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+pytestDir)
                connectIssueLogger()
                self.small = [
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
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        @ut.skip("temporarily")
        def test_RasterCalculation(self):
            rc = RasterCoverage("n000302.mpr")
            rctif = RasterCoverage("n0.mpr")
            self.assertTrue(rc and rctif, msg="couldn't load RasterCoverages")
            self.assertEqual(rc.name(), "n000302.mpr")
            self.assertEqual(rctif.name(), "n0.mpr")
            self.assertEqual(rc.pix2value(342, 342, 0), 29.0)
            self.assertEqual(rc.coord2value(Coordinate(-1871900.47, 1874237.55,0)), 29.0)
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

            self.assertTrue(aa1.setConnection(workingDir + pytestDir + "/aa1", "GTiff", "gdal", IlwisObject.cmOUTPUT),
                            msg="setConnection file failed!")
            aa1.store()

        def test_PixelIterator(self):
            rcl = RasterCoverage("small.mpl")
            self.assertTrue(bool(rcl), msg="couldn't load small.mpl")
            self.assertEqual(rcl.name(), "small.mpl")

            i = 0
            for v in rcl:
                self.assertEqual(v, self.small[i])
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
                self.assertEqual(next(it), self.small[i])

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
            self.assertTrue(Pixel(1, 1, 1) in rit)
            self.assertFalse(Pixel(5, 1, 1) in rit)
            self.assertFalse(rit3 == rit2)
            self.assertEqual(float(rit2), 22.0)
            self.assertEqual(rit[23], 120.0)
            self.assertEqual(float(rit), 120.0)
            rit[23] = 434
            self.assertEqual(rit[23], 434)
            rit2 = PixelIterator(rit[Pixel(2, 2, 2)])
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
            self.assertTrue(Pixel(1, 1, 1) in bit)
            self.assertFalse(Pixel(0, 0, 0) in bit)
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

        def test_NumPy(self):
            try:
                import numpy as np
            except ImportError:
                self.skipTest("numpy not available")

            rcl = RasterCoverage("small.mpl")
            it = iter(rcl)
            a = np.fromiter(it, np.float, it.box().size().linearSize())
            b = np.equal(self.small, a)
            self.assertTrue(all(v for v in b))

            # this is !!experimental!! direct access for NumPy. We don't encourage to use PixelIterator.asBuffer()
            # because so far no consistency check is done on memory access and the blocks of a raster(-list) are not
            # contiguous, so make sure numpy.ndarrays (views) point only to contiguous memory by adjusting the size
            # to the actual block size of the current block (interface still missing)

            rc = RasterCoverage("n000302.mpr")
            it2 = PixelIterator(rc)
            self.assertEqual(1152*1152, it2.box().size().linearSize())
            bu = np.frombuffer(it2.asBuffer(), np.float, 500*1152, 0)  # numpy-array only from first block (500 lines)
            self.assertTrue(all(0 <= v <= 255 for v in bu))


    #@ut.skip("temporarily")
    class TestExample(ut.TestCase):  # and martins solution proposal <== example code for presentation
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + exampleDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
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

            self.assertTrue(polygongrid.setConnection(workingDir + exampleDir + "/polygongrid", "polygonmap", "ilwis3",
                                                  IlwisObject.cmOUTPUT))
            polygongrid.store()

    #@ut.skip("temporarily")
    class TestBaby(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + babyDir)
                connectIssueLogger()
            except IlwisException:
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_helloRaster(self):
            rc = RasterCoverage("n000302.mpr")
            res = Engine.do("aggregateraster", rc, "Avg", 10, True)
            self.assertTrue(res.setConnection(workingDir+babyDir+"/avg_n000302", "map", "ilwis3", IlwisObject.cmOUTPUT))
            res.store()

        def test_helloFeature(self):
            fc_soils = FeatureCoverage("aafsoils.shp")
            fc_soils.setCoordinateSystem(CoordinateSystem("wgs.csy"))
            count = 0
            fc_soils.addAttribute("selected", "boolean")
            for feature in fc_soils:
                if float(feature["AREA"]) == 0.123:
                    count += 1
                    self.assertRegex(str(feature.geometry()), r"POLYGON\s\(\([\s\.\-\,0-9]*\)\)",
                                     msg="wrong WKT representation of geometry!")
                    feature["selected"] = True
                else:
                    feature["selected"] = False
            self.assertEqual(count, 3, msg="wrong count value!")
            fc_soils.setConnection(workingDir + babyDir + "/soils_select", "polygonmap", "ilwis3", IlwisObject.cmOUTPUT)
            fc_soils.store()

    ##@ut.skip("temporarily")
    class TestWorld(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + worldDir)
                connectIssueLogger()
            except IlwisException:
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_prepareHelloWorld(self):
            world = FeatureCoverage("ne_110m_admin_0_countries.shp")
            self.assertTrue(bool(world))
            self.assertEqual(177, world.featureCount())
            world.setCoordinateSystem(CoordinateSystem("countries.csy"))
            world.setConnection(workingDir + worldDir + "/countries", "polygonmap", "ilwis3", IlwisObject.cmOUTPUT)
            world.store()
            self.assertEqual(177, world.featureCount())

        def test_halloWorld(self):
            world = FeatureCoverage(workingDir + worldDir + "/countries.mpa")
            if bool(world):
                population = {}
                self.assertEqual(286, world.featureCount())
                for country in world:
                    name = str(country["iso_a2"])
                    if name not in population:
                        population[name] = int(country["pop_est"])
            else:
                self.skipTest("countries.mpa is missing")
            # print(sorted(population.items(), key=lambda x: x[1]))

    #here you can chose which test case will be executed
    if __name__ == "__main__":
        ut.main(verbosity=2)

except ImportError as e:
    print(e)
