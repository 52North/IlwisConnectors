#!/usr/bin python
# -*- coding: utf-8 -*-
from unittest.case import skip

try:
    from ilwisobjects import *

    workingDir = "file:///D:/Profiles/KiesslingHG/ILWIS/testdata/pytest"
    babyDir = "/baby"
    exampleDir = "/example"
    worldDir = "/world"
    tempDir = "/temp"
    featureDir = "/feature"
    rasterDir = "/raster"

    import unittest as ut

    #@ut.skip("temporarily")
    class TestTable(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+featureDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        def test_AttributeTable(self):
            fc = FeatureCoverage("rainfall.shp")
            t = fc.attributeTable()
            self.assertEqual(t.name(), "rainfall.shp")
            self.assertEqual(
                ('RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE', 'JULY', 'AUGUST',
                 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'feature_id'),
                t.columns()
            )
            # TODO bug fix in GdalLoader
            # self.assertEqual((48, 46, 86, 89, 44, 40, 44, 85, 89, 0, 0, 0, 0), t.column("OCTOBER"))

        def test_StandaloneIlwis3Table(self):
            t = Table("rainfall.tbt")
            self.assertEqual("rainfall.tbt", t.name())
            self.assertEqual(12, t.recordCount())
            self.assertEqual(14, t.columnCount())
            t.addColumn("newColumn", "value")
            self.assertEqual(15, t.columnCount())
            self.assertEqual(
                ('january', 'february', 'march', 'april', 'may', 'june', 'july', 'august', 'september', 'october',
                 'november', 'december', 'newcol', 'ident', 'newColumn'),
                t.columns()
            )
            self.assertEqual((4, 5, 6), t.select("march < 100 AND march != 87"))
            self.assertEqual((9, 10, 11), t.select("march == "+str(Const.rUNDEF) ))
            self.assertEqual(13, t.columnIndex("ident"))
            self.assertEqual(81, t.cell("march", 4))
            self.assertEqual(2, t.columnIndex("march"))
            self.assertEqual([81, 76, 79], [t.cell(2, rec) for rec in t.select("march < 100 AND march != 87")])
            self.assertEqual(Const.rUNDEF, t.cell("newColumn", 0))
            t.setCell("newColumn", 0, 32)
            self.assertEqual(32, t.cell("newColumn", 0))
            disconnectIssueLogger()
            t.setCell("newColumn", 0, "text")
            connectIssueLogger()
            self.assertEqual(Const.rUNDEF, t.cell("newColumn", 0))
            self.assertEqual((87, 87, 160, 150, 81, 76, 79, 155, 160, -1e+308, -1e+308, -1e+308), t.column("march"))
            self.assertEqual((87, 87, 160, 150, 81, 76, 79, 155, 160, -1e+308, -1e+308, -1e+308), t.column(2))
            self.assertEqual((175, 165, 160, 78, 54, 35, 16, 4, 20, 86, 173, 181, 340, 2, -1e+308), t.record(2))

        def testStandaloneGdalTable(self):
            t = Table("rainfall.shp")
            self.assertTrue(bool(t))
            self.assertFalse(t.isInternal(), msg="created a new table object with that name!!")
            self.assertEqual(t.name(), "rainfall.shp")
            expected = ('RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE', 'JULY', 'AUGUST',
                 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT')
            actual = t.columns()
            self.assertTrue(all(expected[i] == actual[i] for i in range(len(expected))))  # if rainfall.shp was loaded before "feature_id" might be the last field
            fc = FeatureCoverage("rainfall.shp")
            self.assertTrue(bool(fc))
            self.assertFalse(fc.isInternal(), msg="created a new table object with that name!!")
            self.assertEqual(
                ('RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE', 'JULY', 'AUGUST',
                 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'feature_id'),
                fc.attributeTable().columns()
            )

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
            self.assertEqual(g.coordinateSystem(), self.csy)

        def test_Transform(self):
            pm = CoordinateSystem("code=epsg:3857")
            self.assertEqual("WGS 84 / Pseudo-Mercator", pm.name())
            wgs = CoordinateSystem("code=epsg:4326")
            self.assertEqual("WGS 84", wgs.name())
            gk2 = CoordinateSystem("code=epsg:31466")
            self.assertEqual("DHDN / 3-degree Gauss-Kruger zone 2", gk2.name())
            gk5 = CoordinateSystem("code=epsg:3329")
            self.assertEqual("Pulkovo 1942(58) / 3-degree Gauss-Kruger zone 5", gk5.name())
            etrs_laea = CoordinateSystem("code=epsg:3035")
            self.assertEqual("ETRS89 / LAEA Europe", etrs_laea.name())

            g = Geometry("POINT(766489.647 6840642.671)", pm)
            g1 = g.transform(gk5)
            self.assertEqual("POINT (4945949.0809892630204558 5819418.3890127958729863)", g1.toWKT())
            g2 = g1.transform(wgs)
            self.assertEqual("POINT (6.8854937241464658 52.2235232302096932)", g2.toWKT())
            g1 = g2.transform(gk5)
            self.assertEqual("POINT (4945949.0857109790667892 5819418.3856139155104756)", g1.toWKT())
            g1 = g2.transform(gk2)
            self.assertEqual("POINT (2560502.5977886635810137 5787988.6849722778424621)", g1.toWKT())
            g1 = g2.transform(etrs_laea)
            self.assertEqual("POINT (4108242.5654639988206327 3239420.9621137753129005)", g1.toWKT())

        def test_Envelope(self):
            g = Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
            e = g.envelope()
            self.assertEqual("POLYGON(1 1,10 10)", str(e))
            g = Geometry("POINT(1 1 1)", self.csy)
            e = g.envelope()
            self.assertEqual("POLYGON(1 1,1 1)", str(e),
                             msg="z's are always 0, since boost can only manage 2D geometries until now")
            g = Geometry("POINT(766489.647 6840642.671)", self.csy)
            self.assertEqual("POLYGON(766490 6.84064e+06,766490 6.84064e+06)", str(g.envelope()))
            self.assertEqual("POINT (766489.6469999999972060 6840642.6710000000894070)", str(g))

        def test_BadWKT(self):
            g = True
            with self.assertRaises(SyntaxError):
                g = Geometry("Pihkdjfhskdf", self.csy)
            self.assertTrue(bool(g))
            g = Geometry("POINT EMPTY", self.csy)
            self.assertTrue(bool(g))
            g.fromWKT("POINT(5 5)")
            self.assertTrue(bool(g))
            with self.assertRaises(SyntaxError):
                g.fromWKT("fdsfsds")

        def test_SimpleFeatures(self):
            p = Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
            self.assertEqual(str(p), "POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 \
10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, \
1.0000000000000000 1.0000000000000000))")
            self.assertTrue(bool(p))
            pin = Geometry("POINT(5 5)",self.csy)
            self.assertEqual(str(pin), "POINT (5.0000000000000000 5.0000000000000000)")
            self.assertTrue(bool(pin))
            self.assertTrue(p.contains(pin))
            self.assertTrue(pin.within(p))
            geom = Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
            self.assertTrue(p.isSimple())
            self.assertTrue(p.within(geom))
            self.assertTrue(p.contains(geom))
            self.assertFalse(p.disjoint(geom))
            self.assertFalse(p.touches(geom))
            self.assertTrue(p.intersects(geom))
            self.assertFalse(p.crosses(geom))
            self.assertFalse(p.overlaps(geom))
            self.assertTrue(p.equals(geom))
            self.assertTrue(p.equalsExact(geom, 0))
            self.assertTrue(p.covers(geom))
            self.assertTrue(p.coveredBy(geom))
            self.assertFalse(p.relate(geom, "T*T***T**"))  # overlaps
            self.assertTrue(p.relate(geom, "T*F**F***"))  # within
            self.assertEqual(p.distance(geom), 0)
            self.assertEqual(p.getArea(), 81)
            self.assertEqual(p.getLength(), 36)
            self.assertTrue(p.isWithinDistance(geom, 0))
            self.assertEqual(
                "POLYGON ((1.0000000000000000 0.0000000000000000, 0.8049096779838661 0.0192147195967707, \
0.6173165676349055 0.0761204674887151, 0.4444297669803942 0.1685303876974572, 0.2928932188134502 0.2928932188134548, \
0.1685303876974533 0.4444297669803999, 0.0761204674887127 0.6173165676349116, 0.0192147195967693 0.8049096779838725, \
0.0000000000000000 1.0000000000000000, 0.0000000000000000 10.0000000000000000, 0.0192147195967696 10.1950903220161280, \
0.0761204674887133 10.3826834323650896, 0.1685303876974547 10.5555702330196013, 0.2928932188134525 10.7071067811865479, \
0.4444297669803980 10.8314696123025449, 0.6173165676349103 10.9238795325112861, 0.8049096779838718 10.9807852804032304, \
1.0000000000000000 11.0000000000000000, 10.0000000000000000 11.0000000000000000, 10.1950903220161280 10.9807852804032304, \
10.3826834323650896 10.9238795325112861, 10.5555702330196031 10.8314696123025449, 10.7071067811865479 10.7071067811865479, \
10.8314696123025449 10.5555702330196013, 10.9238795325112861 10.3826834323650896, 10.9807852804032304 10.1950903220161280, \
11.0000000000000000 10.0000000000000000, 11.0000000000000000 1.0000000000000000, 10.9807852804032304 0.8049096779838718, \
10.9238795325112861 0.6173165676349102, 10.8314696123025449 0.4444297669803978, 10.7071067811865479 0.2928932188134525, \
10.5555702330196031 0.1685303876974548, 10.3826834323650896 0.0761204674887133, 10.1950903220161280 0.0192147195967696, \
10.0000000000000000 0.0000000000000000, 1.0000000000000000 0.0000000000000000))",
                str(p.buffer(1)))
            self.assertEqual("POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 \
10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, \
1.0000000000000000 1.0000000000000000))", str(p.convexHull()))
            self.assertEqual("POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 \
10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, \
1.0000000000000000 1.0000000000000000))", str(p.intersection(geom)))
            self.assertEqual("POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 \
10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, \
1.0000000000000000 1.0000000000000000))", str(p.Union(geom)))
            self.assertEqual("GEOMETRYCOLLECTION EMPTY", str(p.difference(geom)))
            self.assertEqual("GEOMETRYCOLLECTION EMPTY", str(p.symDifference(geom)))

    #@ut.skip("temporarily")
    class TestUtil(ut.TestCase):
        def test_PixelCoordinate(self):
            p = Pixel(4, 5)
            self.assertTrue(bool(p))
            self.assertEqual(str(p), "pixel(4,5)")
            p *= 3
            self.assertTrue(p == Pixel(12, 15))
            self.assertEqual(str(p), "pixel(12,15)")
            p /= 4
            self.assertEqual(str(p), "pixel(3,3)")
            self.assertTrue(p != Pixel(3, 4))
            self.assertFalse(p.is3D())
            v = Pixel(4, 5, 6)
            self.assertEqual(str(v), "pixel(4,5,6)")
            self.assertTrue(v.is3D())
            self.assertEqual(v.x, 4)
            v.x = 32
            self.assertEqual(v.x, 32)
            v.y = 32
            self.assertEqual(v.y, 32)
            v.z = 32
            self.assertEqual(v.z, 32)
            self.assertEqual(str(v), "pixel(32,32,32)")

            c = Coordinate(-1871900.47, 1874237.55)
            self.assertTrue(bool(c))
            self.assertFalse(c.is3D())
            self.assertEqual(str(c), "coordinate(-1871900.470000,1874237.550000)")
            c *= 6
            self.assertTrue(c == Coordinate(-11231402.820000, 11245425.300000))
            self.assertEqual(str(c), "coordinate(-11231402.820000,11245425.300000)")
            c /= 6
            self.assertEqual(str(c), "coordinate(-1871900.470000,1874237.550000)")
            self.assertFalse(c.is3D())
            c = Coordinate(-1871900.47, 1874237.55, 2)
            self.assertTrue(c.is3D())
            self.assertEqual(str(c), "coordinate(-1871900.470000,1874237.550000,2.000000)")
            self.assertEqual(c.x, -1871900.47)
            c.x = 4.323
            self.assertEqual(c.x, 4.323)


        #@ut.skip("temporarily")
        def test_SizeSizeD(self):
            sz = Size(2, 4, 5)
            self.assertTrue(bool(sz))
            self.assertEqual(str(sz), "Size(2, 4, 5)")
            self.assertEqual(sz.linearSize(), 2*4*5)
            sz *= 2
            self.assertEqual(str(sz), "Size(4, 8, 10)")
            sz += Size(4, 4, 0)
            self.assertTrue(sz == Size(8, 12, 10))
            sz -= Size(1, 1, 0)
            self.assertTrue(sz == Size(7, 11, 10))
            sz.xsize = 3
            self.assertEqual(sz.xsize, 3)
            self.assertTrue(Pixel(1, 1, 1) in sz)
            self.assertFalse(Pixel(5, 5, 5) in sz)
            sz = SizeD(sz)
            self.assertEqual(str(sz), "Size(3, 11, 10)")
            self.assertTrue(sz == SizeD(3., 11., 10.))
            with self.assertRaises(TypeError, msg="Size and SizeD should be incomparable!"):
                sz == Size(3, 11, 10)
            self.assertTrue(Pixel(2, 10, 9) in sz)
            self.assertTrue(PixelD(2., 10., 9.) in sz)
            self.assertTrue(Coordinate(2., 10., 9.) in sz)
            self.assertEqual(str(sz), "Size(3, 11, 10)")
            sz -= SizeD(100., 100.)  # == SizeD(100., 100., 1.)
            self.assertEqual(str(sz), "Size(0, 0, 9)")
            self.assertFalse(bool(sz))
            self.assertFalse(bool(SizeD(10, 10, 0)))
            sz -= SizeD(10, 10, 0)  # calculation with invalid sz == Size(0, 0, 9) results in Size(0, 0, 0)
            self.assertEqual(str(sz), "Size(0, 0, 0)")
            sz = SizeD(2.5, 4.4, 5.1)
            self.assertTrue(sz == SizeD(2.5, 4.4, 5.1))
            self.assertEqual(str(sz), "Size(2.5, 4.4, 5.1)")
            self.assertEqual(sz.linearSize(), int(2.5)*int(4.4)*int(5.1))
            sz *= 2
            self.assertEqual(str(sz), "Size(5, 8.8, 10.2)")
            sz = Size(32, 43)
            self.assertEqual(str(sz), "Size(32, 43, 1)")

        ##@ut.skip("temporarily")
        def test_BoxEnvelope(self):
            b = Box(Pixel(3, 4, 5), Pixel(4, 5, 6,))
            self.assertTrue(bool(b))
            self.assertTrue(b.is3D())
            self.assertEqual(str(b), "POLYGON(3 4 5,4 5 6)")
            self.assertTrue(b == Box("POLYGON(3 4 5,4 5 6)"))
            self.assertEqual(str(b.minCorner()), "pixel(3,4,5)")
            b.minCorner().x = 39
            self.assertEqual(str(b.minCorner()), "pixel(3,4,5)")
            self.assertTrue(b.minCorner() == Pixel(3, 4, 5))
            self.assertEqual(str(b.maxCorner()), "pixel(4,5,6)")
            self.assertEqual(str(b.size()), "Size(2, 2, 2)")
            self.assertTrue(b.size() == Size(2, 2, 2))
            self.assertEqual(b.size().linearSize(), 2*2*2)
            b = Box(Pixel(2, 3), Pixel(4, 5))
            self.assertEqual(str(b), "POLYGON(2 3,4 5)")
            self.assertFalse(b.is3D())
            self.assertTrue(bool(b))

            env = Envelope(Coordinate(3.6111119, 4.7, 5.9), Coordinate(4.7, 5.8, 6.9))
            self.assertEqual(str(env), "POLYGON(3.61111 4.7 5.9,4.7 5.8 6.9)")
            self.assertEqual(str(env.size()), "Size(2.08889, 2.1, 2)")
            env = Envelope("POLYGON(3.6111119 4.7 5.9,4.7 5.8 6.9)")
            self.assertEqual(str(env), "POLYGON(3.61111 4.7 5.9,4.7 5.8 6.9)")
            self.assertEqual(str(env.size()), "Size(2.08889, 2.1, 2)")
            self.assertFalse(env.size() == SizeD(2.08889, 2.1, 2.))  # bug on Python float precision
            env = Envelope(env.size())
            self.assertEqual("POLYGON(0 0 0,1.08889 1.1 1)", str(env))
            env = Envelope(Coordinate(3, 4, 5), Coordinate(4, 5, 6,))
            self.assertEqual(str(env), "POLYGON(3 4 5,4 5 6)")
            env = Envelope(env.size())
            self.assertEqual(str(env), "POLYGON(0 0 0,1 1 1)")
            self.assertTrue(Coordinate(.5, .5, .5) in env)
            self.assertTrue(env in env)

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
            fc = FeatureCoverage(workingDir+"/noneexistentDir/nonexistent.file")
            self.assertFalse(bool(fc))
            connectIssueLogger()

    #@ut.skip("temporarily")
    class TestPyVariant(ut.TestCase):
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
            with self.assertRaises(ValueError, msg="did not overflow unsigned long long int"):
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
                Engine.setWorkingCatalog(workingDir)
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
            oper = ('setvaluerange', 'binarymathtable', 'selection', 'mastergeoreference', 'binarymathfeatures',
                    'binarymathraster', 'selection', 'iff', 'stringfind', 'stringsub', 'stringreplace', 'rastersize',
                    'text2output', 'coord2pixel', 'coordinate', 'pixel', 'pixel2coord', 'selection', 'assignment',
                    'sin', 'cos', 'tan', 'asin', 'acos', 'atan', 'log10', 'ln', 'abs', 'sqrt', 'ceil', 'floor', 'sgn',
                    'cosh', 'sinh', 'binarylogicalraster', 'iff', 'rastervalue', 'resample', 'gridding', 'script',
                    'aggregateraster', 'areanumbering', 'cross', 'linearstretch', 'linearrasterfilter',
                    'rankorderrasterfilter'
                    )
            self.assertTupleEqual(oper, ops)
            self.assertEqual("gridsize(rastercoverage,xsize|ysize|zsize)", e.operationMetaData("rastersize"))
            self.assertEqual("gridding(coordinatesyste,top-coordinate,x-cell-size, y-cell-size, horizontal-cells, vertical-cells)",
                             e.operationMetaData("gridding"))
            self.assertEqual("iffraster(featurecoverage,outputchoicetrue, outputchoicefalse)\n\
iffraster(rastercoverage,outputchoicetrue, outputchoicefalse)", e.operationMetaData("iff"))

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
                Engine.setWorkingCatalog(workingDir+featureDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        def test_GPXFromFile(self):
            # fc = FeatureCoverage("favourites.gpx")
            fc = FeatureCoverage("test.gpx")
            self.assertFalse(fc.isInternal())
            self.assertEqual(fc.featureCount(), 0)
            for f in fc:
                print(f.geometry())

        ##@ut.skip("temporarily")
        def test_FeatureCoverage(self):
            fc = FeatureCoverage("rainfall.shp")
            self.assertTrue(fc, msg="FeatureCoverage(rainfall.shp) not loaded correctly!")
            self.assertEqual(fc.name(), "rainfall.shp", msg="internal FeatureCoverage name wrong!")
            self.assertEqual(fc.featureCount(), 13, msg="feature count wrong")

            self.assertTrue(fc.addAttribute("sum", "value"), msg="FeatureCoverage.addAttribute failed!")
            att = fc.attributes()
            self.assertTupleEqual(att, (
                'RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE', 'JULY',
                'AUGUST', 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'sum'
            ), msg="wrong list of attributes!")
            self.assertEqual(len(att), 16, msg="wrong number of attributes")
            g = Geometry("POINT(5.4 6 9.0)", fc.coordinateSystem())
            newfeature = fc.newFeature(g)
            self.assertTrue(bool(newfeature), msg="newfeature creation failed!")
            for c in att:
                newfeature[c] = 12
                self.assertEqual(int(newfeature[c]), 12, msg="new value of feature attribute not correct!")

            self.assertEqual(fc.featureCount(), 14, msg="new feature count wrong")

        ##@ut.skip("temporarily")
        def test_Feature(self):
            fc = FeatureCoverage("rainfall.shp")
            self.assertTrue(fc, msg="FeatureCoverage(rainfall.shp) not loaded correctly!")
            it = iter(fc)
            f = next(it)
            f.geometry().fromWKT("POINT(5.4 6 9.0)")
            self.assertEqual(f.geometry().toWKT(), "POINT (5.4000000000000004 6.0000000000000000)", msg="unsuccessfully altered geometry")
            with self.assertRaises(IndexError, msg="no IndexError on call of wrong attribute"):
                v = f["RAINFAL"]
            v = f["RAINFALL"]
            self.assertEqual(str(v), "UMSS", msg="wrong attribute value")
            with self.assertRaises(ValueError, msg="no TypeError on attempt to convert non-numerical string to int"):
                self.assertEqual("it wont be a number", int(v))
            f["RAINFALL"] = 12
            self.assertEqual(12, int(f["RAINFALL"]), msg="no real type check here since it could be converted back and forth")

        def test_FeatureIterator(self):
            fc = FeatureCoverage("rainfall.shp")
            self.assertTrue(fc, msg="FeatureCoverage(rainfall.shp) not loaded correctly!")
            summ = 0
            for f in fc:
                summ += f.attribute("MAY", 0)
                f["sum"] = summ
                self.assertRegex(str(f), r"Feature\([0-9]*\)", msg="wrong feature representation")
                self.assertRegex(str(f.geometry()),
                                 r"POINT\s\(([0-9\.\-]+|\-1e\+308|5\.4)\s([0-9\.\-]+|\-1e\+308|[0-9]+\.[0-9]+e\+[0-9]+)\)",
                                 msg="wrong geometry representation")
            if fc.featureCount() == 14:
                self.assertEqual(summ, 298.0, msg="wrong sum over rainfall in MAY!")
            else:
                self.assertEqual(summ, 286.0, msg="wrong sum over rainfall in MAY!")
            del summ
            it = iter(fc)
            self.assertTrue(bool(it))
            self.assertEqual(str(it), "FeatureIterator for rainfall.shp")
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

        def test_DateTimeAttribute(self):
            fc = FeatureCoverage("drainage.shp")
            it = iter(fc)
            f = next(it)
            v = f["date"]
            self.assertEqual("2014-02-17 00:00:00", str(v))
            import datetime
            type(v) is datetime.datetime
            self.assertEqual(v, datetime.datetime(2014, 2, 17))
            f["date"] = datetime.datetime(2014, 2, 27)
            self.assertEqual("2014-02-17 00:00:00", str(f["date"]))


        def test_loadGDALstoreGDAL(self):
            # polygons
            world = FeatureCoverage("ne_110m_admin_0_countries.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.setConnection(workingDir+tempDir+"/countries_fromshp.shp", "ESRI Shapefile", "gdal", IlwisObject.cmOUTPUT)
            world.store()
            # points
            world = FeatureCoverage("rainfall.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.setConnection(workingDir+tempDir+"/rainfall_fromshp.shp", "ESRI Shapefile", "gdal", IlwisObject.cmOUTPUT)
            world.store()
            # lines
            world = FeatureCoverage("drainage.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.setConnection(workingDir+tempDir+"/drainage_fromshp.shp", "ESRI Shapefile", "gdal", IlwisObject.cmOUTPUT)
            world.store()

        #@ut.skip("temporarily")
        def test_loadGDALstoreIlwis3(self):
            # polygons
            world = FeatureCoverage("ne_110m_admin_0_countries.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
#            world.setCoordinateSystem(CoordinateSystem("countries.csy"))  # TODO use/copy shp files coordinate system instead
            world.setConnection(workingDir+tempDir+"/countries_fromshp", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
            world.store()
            # points
            world = FeatureCoverage("rainfall.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.setCoordinateSystem(CoordinateSystem("countries.csy"))
            world.setConnection(workingDir+tempDir+"/rainfall_fromshp", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
            world.store()
            self.assertEqual("rainfall.shp", world.attributeTable().name())
            # lines
            world = FeatureCoverage("drainage.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.setCoordinateSystem(CoordinateSystem("countries.csy"))
            world.setConnection(workingDir+tempDir+"/drainage_fromshp", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
            # world.store()  # skiped until Ilwis3Connector can handle datetime format

        # def test_loadIlwis3storeIlwis3(self):
        #     # polygons
        #     world = FeatureCoverage("ne_110m_admin_0_countries.shp")
        #     self.assertTrue(bool(world))
        #     self.assertFalse(world.isInternal())
        #     world.setCoordinateSystem(CoordinateSystem("countries.csy"))
        #     world.setConnection(workingDir+tempDir+"/countries_fromshp", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
        #     world.store()
        #     # points
        #     world = FeatureCoverage("rainfall.shp")
        #     self.assertTrue(bool(world))
        #     self.assertFalse(world.isInternal())
        #     world.setCoordinateSystem(CoordinateSystem("countries.csy"))
        #     world.setConnection(workingDir+tempDir+"/rainfall_fromshp", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
        #     world.store()
        #     # lines
        #     world = FeatureCoverage("drainage.shp")
        #     self.assertTrue(bool(world))
        #     self.assertFalse(world.isInternal())
        #     world.setCoordinateSystem(CoordinateSystem("countries.csy"))
        #     world.setConnection(workingDir+tempDir+"/drainage_fromshp", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
        #     world.store()

    #@ut.skip("temporarily")
    class TestCoordinateSystem(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+featureDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        def test_FromFile(self):
            csy = CoordinateSystem(workingDir+featureDir+"/Cochabamba.csy")
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
            fc = FeatureCoverage(workingDir+featureDir+"/rainfall.shp")
            fc.setCoordinateSystem(cs1)
            self.assertEqual(fc.coordinateSystem().name(), "myProj4CSY",
                             msg="could not alter FeatureCoverage's CoordinateSystem")

        def test_EPSG(self):
            cs2 = CoordinateSystem("code=epsg:23035")
            self.assertTrue(bool(cs2))
            self.assertEqual(cs2.name(), r"ED50 / UTM zone 35N")

        def test_isEqual(self):
            cs1 = CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
            cs2 = CoordinateSystem("code=epsg:23035")
            self.assertTrue(cs1 == cs2)
            self.assertFalse(cs1 != cs2)

    class TestGeoReference(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+rasterDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_standaloneFile(self):
            gr = GeoReference("n000302.grf")
            self.assertTrue(bool(gr))
            self.assertFalse(gr.isInternal())
            self.assertTrue(gr.centerOfPixel())

        def test_fromRaster(self):
            r = RasterCoverage("n000302.mpr")
            gr = r.geoReference()
            self.assertTrue(bool(gr))
            self.assertFalse(gr.isInternal())
            self.assertTrue(r.coordinateSystem() == gr.coordinateSystem())
            self.assertTrue(r.size() == gr.size())
            box = Box(r.size())
            env = gr.box2Envelope(box)
            self.assertEqual("POLYGON(0 0 0,1151 1151 0)", str(box))
            self.assertEqual("POLYGON(-4.60799e+06 -4.59997e+06,4.60001e+06 4.60803e+06)", str(env))
            subenv = Envelope(Coordinate(-1e+06, -1e+06), Coordinate(1e+06, 1e+06))
            subbox = gr.envelope2Box(subenv)
            self.assertEqual("POLYGON(-1e+06 -1e+06,1e+06 1e+06)", str(subenv))
            self.assertEqual("POLYGON(451 451,701 701)", str(subbox))
            self.assertEqual("pixel(536.599,478.436)", str(gr.coord2Pixel(Coordinate(-319195.47, 784540.64))))
            self.assertEqual("coordinate(-315198.248000,780544.506500)", str(gr.pixel2Coord(PixelD(536.599, 478.436))))
            self.assertEqual("coordinate(-319990.248000,784032.506500)", str(gr.pixel2Coord(Pixel(536, 478))))
            # self.assertEqual(0, gr.pixelSize())  # TODO possible bug in GeoRefImplementaion  - nan is not a goo pixelSize!!
            self.assertTrue(gr.centerOfPixel())
            self.assertEqual("POLYGON(536 478,536 478)", str(
                gr.envelope2Box(Envelope(Coordinate(-319195.47, 784540.64), Coordinate(-319990.248000, 784032.506500)))
            ))
            gr.centerOfPixel(False)
            self.assertFalse(gr.centerOfPixel())
            self.assertEqual("POLYGON(536 478,536 478)", str(
                gr.envelope2Box(Envelope(Coordinate(-319195.47, 784540.64), Coordinate(-319990.248000, 784032.506500)))
            ))


    #@ut.skip("temporarily")
    class TestRaster(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+rasterDir)
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
            pix = Pixel(342, 342, 0)
            self.assertTrue(pix in rc.size())
            self.assertEqual(rc.pix2value(pix), 29.0)
            self.assertEqual(rc.coord2value(Coordinate(-1871900.47, 1874237.55, 0)), 29.0)
            self.assertEqual(rctif.pix2value(pix), 29.0)

            aa7 = Engine.do("sin", rc)
            self.assertAlmostEqual(aa7.pix2value(pix), -0.663, 3)

            aa1 = rc + rctif
            self.assertEqual(aa1.pix2value(pix), 29.0 + 29.0)
            aa2 = rc + 2
            self.assertEqual(aa2.pix2value(pix), 29.0 + 2)
            aa3 = 2 + rc
            self.assertEqual(aa3.pix2value(pix), 2 + 29.0)
            aa4 = rc - rctif
            self.assertEqual(aa4.pix2value(pix), 29.0 - 29.0)
            #TODO fix parsing of numeric - raster (don't switch arguments)
            aa5 = 2 - rc  # until now this is parsed as "rc - 2" :(
            #self.assertEqual(aa5.pix2value(pix),2-29.0)
            aa6 = rc - 2
            self.assertEqual(aa6.pix2value(pix), 29.0 - 2)
            aa7 = rc / rctif
            self.assertEqual(aa7.pix2value(pix), 29.0 / 29.0)
            #TODO fix parsing of numeric / raster (don't switch arguments)
            aa8 = 2 / rc  # until now this is parsed as "rc / 2" :(
            #self.assertEqual(aa8.pix2value(pix),2/29.0)
            aa9 = rc / 2
            self.assertAlmostEqual(aa9.pix2value(pix), 29.0 / 2, 1)
            aa10 = rc * rctif
            self.assertEqual(aa10.pix2value(pix), 29.0 * 29.0)
            aa11 = 2 * rc
            self.assertEqual(aa11.pix2value(pix), 2 * 29.0)
            aa12 = rc * 2
            self.assertEqual(aa12.pix2value(pix), 29.0 * 2)

            aa1.setConnection(workingDir + tempDir + "/n000302_frommpr", "GTiff", "gdal", IlwisObject.cmOUTPUT)
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
                        maxval = max(polygon.attribute("maxY", 0), point.attribute("freq_speciesY", 0))
                        polygon.setAttribute("maxY", maxval)

            polygongrid.setConnection(workingDir + exampleDir + "/polygongrid", "vectormap", "ilwis3",IlwisObject.cmOUTPUT)
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
            res.setConnection(workingDir+babyDir+"/avg_n000302", "map", "ilwis3", IlwisObject.cmOUTPUT)
            res.store()

        def test_helloFeature(self):
            fc_soils = FeatureCoverage("aafsoils.shp")
            fc_soils.setCoordinateSystem(CoordinateSystem("wgs.csy"))
            count = 0
            fc_soils.addAttribute("selected", "boolean")
            for feature in fc_soils:
                if feature["AREA"] == 0.123:
                    count += 1
                    self.assertRegex(str(feature.geometry()), r"POLYGON\s\(\([\s\.\-\,0-9]*\)\)",
                                     msg="wrong WKT representation of geometry!")
                    feature["selected"] = True
                else:
                    feature["selected"] = False
            self.assertEqual(count, 3, msg="wrong count value!")
            fc_soils.setConnection(workingDir + babyDir + "/soils_select", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
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
        def test_halloWorld(self):
            world = FeatureCoverage("countries.mpa")
            if bool(world) and not world.isInternal():
                population_ranking = {}
                self.assertEqual(286, world.featureCount())
                for country in world:
                    name = country["iso_a2"]
                    if name not in population_ranking:
                        population_ranking[name] = country["pop_est"]
                # print(sorted(population.items(), key=lambda x: x[1]))
                self.assertEqual(
                    {'OM': 3418085.0, 'HU': 9905596.0, 'HT': 9035536.0, 'HR': 4489409.0, 'ZW': 12619600.0,
                     'HN': 7792854.0, 'RW': 10473282.0, 'CH': 7604467.0, 'JM': 2825928.0, 'RS': 7379339.0,
                     'RO': 22215421.0, 'JP': 127078679.0, 'GY': 772298.0, 'GT': 13276517.0, 'CO': 45644023.0,
                     'GQ': 650702.0, 'GR': 10737428.0, 'GL': 57600.0, 'GM': 1782893.0, 'GN': 10057975.0,
                     'GH': 23832495.0, 'GE': 4615807.0, 'GA': 1514993.0, 'GB': 62262000.0, 'US': 313973000.0,
                     'GW': 1533964.0, 'MN': 3041142.0, 'ML': 12666987.0, 'MM': 48137741.0, 'UZ': 27606007.0,
                     'UY': 3494382.0, 'MG': 20653556.0, 'MD': 4320748.0, 'ME': 672180.0, 'MZ': 21669278.0,
                     'MX': 111211789.0, 'UA': 45700395.0, 'VE': 26814843.0, 'MR': 3129486.0, 'MW': 14268711.0,
                     'FR': 64057792.0, 'ZM': 11862740.0, 'FK': 3140.0, 'FJ': 944720.0, 'FI': 5250275.0,
                     'LV': 2231503.0, 'LB': 4017095.0, 'PY': 6995655.0, '-99': 265100.0, 'PT': 10707924.0,
                     'PS': 4119083.0, 'PR': 3971020.0, 'PL': 38482919.0, 'PH': 97976603.0, 'PK': 176242949.0,
                     'PE': 29546963.0, 'PG': 6057263.0, 'PA': 3360474.0, 'EC': 14573101.0, 'EG': 83082869.0,
                     'EE': 1299371.0, 'EH': -99.0, 'ER': 5647168.0, 'ES': 40525002.0, 'ZA': 49052489.0,
                     'ET': 85237338.0, 'SY': 20178485.0, 'SZ': 1123913.0, 'MK': 2066718.0, 'SR': 481267.0,
                     'SS': 10625176.0, 'SV': 7185218.0, 'SI': 2005692.0, 'SK': 5463046.0, 'SL': 6440053.0,
                     'SN': 13711597.0, 'SO': 9832017.0, 'SA': 28686633.0, 'SB': 595613.0, 'SD': 25946220.0,
                     'SE': 9059651.0, 'LR': 3441790.0, 'DE': 82329758.0, 'LU': 491775.0, 'DK': 5500510.0,
                     'DJ': 516055.0, 'DO': 9650054.0, 'LT': 3555179.0, 'DZ': 34178188.0, 'NC': 227436.0,
                     'NA': 2108665.0, 'MA': 34859364.0, 'NG': 149229090.0, 'NE': 15306252.0, 'VN': 86967524.0,
                     'NI': 5891199.0, 'NO': 4676305.0, 'IT': 58126212.0, 'NL': 16715999.0, 'NP': 28563377.0,
                     'NZ': 4213418.0, 'KP': 22665345.0, 'CI': 20617068.0, 'KR': 48508972.0, 'CL': 16601707.0,
                     'CM': 18879301.0, 'CN': 1338612970.0, 'KW': 2691158.0, 'CA': 33487208.0, 'KZ': 15399437.0,
                     'CD': 68692542.0, 'CF': 4511488.0, 'CG': 4012809.0, 'CY': 531640.0, 'CZ': 10211904.0,
                     'MY': 25715819.0, 'KE': 39002772.0, 'KG': 5431747.0, 'KH': 14494293.0, 'CR': 4253877.0,
                     'CU': 11451652.0, 'VU': 218519.0, 'UG': 32369558.0, 'JO': 6342948.0, 'RU': 140041247.0,
                     'QA': 833285.0, 'BG': 7204687.0, 'BF': 15746232.0, 'BE': 10414336.0, 'BD': 156050883.0,
                     'BA': 4613414.0, 'BO': 9775246.0, 'BN': 388190.0, 'BJ': 8791832.0, 'BI': 8988091.0,
                     'BW': 1990876.0, 'BT': 691141.0, 'BS': 309156.0, 'BR': 198739269.0, 'BZ': 307899.0,
                     'BY': 9648533.0, 'LK': 21324791.0, 'TR': 76805524.0, 'TT': 1310000.0, 'TW': 22974347.0,
                     'LA': 6834942.0, 'TZ': 41048532.0, 'LY': 6310434.0, 'IL': 7233701.0, 'YE': 23822783.0,
                     'TD': 10329208.0, 'TG': 6019877.0, 'TF': 140.0, 'TH': 65905410.0, 'LS': 2130819.0,
                     'TJ': 7349145.0, 'TM': 4884887.0, 'TL': 1131612.0, 'TN': 10486339.0, 'AO': 12799293.0,
                     'AL': 3639453.0, 'AM': 2967004.0, 'IR': 66429284.0, 'IS': 306694.0, 'IQ': 31129225.0,
                     'AF': 28400000.0, 'AE': 4798491.0, 'ID': 240271522.0, 'IE': 4203200.0, 'AZ': 8238672.0,
                     'IN': 1166079220.0, 'AT': 8210281.0, 'AU': 21262641.0, 'AR': 40913584.0,'AQ': 3802.0},
                    population_ranking)
            else:
                self.skipTest("countries.mpa is missing")

        def test_AttributeTable(self):
            t = Table("countries.tbt")
            self.assertTrue(bool(t))
            self.assertFalse(t.isInternal())


    #here you can chose which test case will be executed
    if __name__ == "__main__":
        ut.main(verbosity=2)

except ImportError as e:
    print(e)
