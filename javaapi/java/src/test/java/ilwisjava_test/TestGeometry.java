package ilwisjava_test;

import static org.junit.Assert.*;

import java.math.BigInteger;

import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestGeometry {
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}

	@Test
	public void geometry() {
		CoordinateSystem csy = new CoordinateSystem("code=epsg:5464");
		Geometry g = new Geometry("POINT(5.4 6 9.0)", csy);
		assertEquals("POINT (5.4000000000000004 6.0000000000000000)",
				g.toString());
		assertEquals("POINT (5.4000000000000004 6.0000000000000000)", g.toWKT());
		assertTrue(g.isValid());
		assertEquals(BigInteger.valueOf(1), g.ilwisType());
		assertEquals("Sibun Gorge 1922", g.coordinateSystem().name());
		assertTrue(g.coordinateSystem().equals(csy));
	}

	@Test
	public void transform() {
		CoordinateSystem pm = new CoordinateSystem("code=epsg:3857");
		assertEquals("WGS 84 / Pseudo-Mercator", pm.name());
		CoordinateSystem wgs = new CoordinateSystem("code=epsg:4326");
		assertTrue(wgs.name().contains("WGS 84"));
		CoordinateSystem gk2 = new CoordinateSystem("code=epsg:31466");
		assertEquals("DHDN / 3-degree Gauss-Kruger zone 2", gk2.name());
		CoordinateSystem gk5 = new CoordinateSystem("code=epsg:3329");
		assertEquals("Pulkovo 1942(58) / 3-degree Gauss-Kruger zone 5",
				gk5.name());
		CoordinateSystem etrs_laea = new CoordinateSystem("code=epsg:3035");
		assertEquals("ETRS89 / LAEA Europe", etrs_laea.name());
		CoordinateSystem proj4 = new CoordinateSystem(
				"code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs");

		Geometry g = new Geometry("POINT(766489.647 6840642.671)", pm);
		Geometry g1 = g.transform(gk5);
		assertEquals(
				"POINT (4945949.0809892630204558 5819418.3890127958729863)",
				g1.toWKT());
		Geometry g2 = g1.transform(wgs);
		assertEquals("POINT (6.8854937241464658 52.2235232302096932)",
				g2.toWKT());
		g1 = g2.transform(gk5);
		assertEquals(
				"POINT (4945949.0857109790667892 5819418.3856139155104756)",
				g1.toWKT());
		g1 = g2.transform(gk2);
		assertEquals(
				"POINT (2560502.5977886635810137 5787988.6849722778424621)",
				g1.toWKT());
		g1 = g2.transform(etrs_laea);
		assertEquals(
				"POINT (4108242.5654639988206327 3239420.9621137753129005)",
				g1.toWKT());
		Geometry g3 = g.transform(proj4);
		assertEquals(
				"POINT (-866479.8875676055904478 5979085.6121951946988702)",
				g3.toWKT());
	}

	@Test
	public void envelope() {
		CoordinateSystem csy = new CoordinateSystem("code=epsg:5464");
		Geometry g = new Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", csy);
		Envelope e = g.envelope();
		assertEquals("1 1 10 10", e.toString());
		g = new Geometry("POINT(1 1 1)", csy);
		e = g.envelope();
		assertEquals("1 1 1 1", e.toString());

		g = new Geometry("POINT(766489.647 6840642.671)", csy);
		assertEquals("766490 6.84064e+06 766490 6.84064e+06", g.envelope()
				.toString());
		assertEquals(
				"POINT (766489.6469999999972060 6840642.6710000000894070)",
				g.toString());
	}

	@Test
	public void badKWT() {
		CoordinateSystem csy = new CoordinateSystem("code=epsg:5464");
		Boolean except = false;
		try {
			Geometry g = new Geometry("dgjfdkgkjd", csy);
			g.isValid();
		} catch (Exception e) {
			assertEquals("ParseException: Unknown type: 'DGJFDKGKJD'",
					e.getMessage());
			except = true;
		}
		assertTrue(except);
		Geometry g = new Geometry("POINT EMPTY", csy);
		assertTrue(g.isValid());
		g.fromWKT("POINT(5 5)");
		assertTrue(g.isValid());
		except = false;
		try {
			g.fromWKT("fdsfsds");
		} catch (Exception e) {
			assertEquals("ParseException: Unknown type: 'FDSFSDS'",
					e.getMessage());
			except = true;
		}
		assertTrue(except);
	}

	@Test
	public void simpleFeatures() {
		CoordinateSystem csy = new CoordinateSystem("code=epsg:5464");
		Geometry p = new Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", csy);
		assertEquals(
				p.toString(),
				"POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 "
						+ "10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, "
						+ "1.0000000000000000 1.0000000000000000))");
		assertTrue(p.isValid());
		Geometry pin = new Geometry("POINT(5 5)", csy);
		assertEquals(pin.toString(),
				"POINT (5.0000000000000000 5.0000000000000000)");
		assertTrue(pin.isValid());

		assertTrue(p.contains(pin));
		assertTrue(pin.within(p));
		Geometry geom = new Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", csy);
		assertTrue(p.isSimple());
		assertTrue(p.within(geom));
		assertTrue(p.contains(geom));
		assertFalse(p.disjoint(geom));
		assertFalse(p.touches(geom));
		assertTrue(p.intersects(geom));
		assertFalse(p.crosses(geom));
		assertFalse(p.overlaps(geom));
		assertTrue(p.equals(geom));
		assertTrue(p.equalsExact(geom, 0));
		assertTrue(p.covers(geom));
		assertTrue(p.coveredBy(geom));
		assertFalse(p.relate(geom, "T*T***T**")); // overlaps
		assertTrue(p.relate(geom, "T*F**F***")); // within
		assertEquals(p.distance(geom), 0, TestUtil.precision);
		assertEquals(p.getArea(), 81, TestUtil.precision);
		assertEquals(p.getLength(), 36, TestUtil.precision);
		assertTrue(p.isWithinDistance(geom, 0));
		assertEquals(
				"POLYGON ((1.0000000000000000 0.0000000000000000, 0.8049096779838661 0.0192147195967707, "
						+ "0.6173165676349055 0.0761204674887151, 0.4444297669803942 0.1685303876974572, 0.2928932188134502 0.2928932188134548, "
						+ "0.1685303876974533 0.4444297669803999, 0.0761204674887127 0.6173165676349116, 0.0192147195967693 0.8049096779838725, "
						+ "0.0000000000000000 1.0000000000000000, 0.0000000000000000 10.0000000000000000, 0.0192147195967696 10.1950903220161280, "
						+ "0.0761204674887133 10.3826834323650896, 0.1685303876974547 10.5555702330196013, 0.2928932188134525 10.7071067811865479, "
						+ "0.4444297669803980 10.8314696123025449, 0.6173165676349103 10.9238795325112861, 0.8049096779838718 10.9807852804032304, "
						+ "1.0000000000000000 11.0000000000000000, 10.0000000000000000 11.0000000000000000, 10.1950903220161280 10.9807852804032304, "
						+ "10.3826834323650896 10.9238795325112861, 10.5555702330196031 10.8314696123025449, 10.7071067811865479 10.7071067811865479, "
						+ "10.8314696123025449 10.5555702330196013, 10.9238795325112861 10.3826834323650896, 10.9807852804032304 10.1950903220161280, "
						+ "11.0000000000000000 10.0000000000000000, 11.0000000000000000 1.0000000000000000, 10.9807852804032304 0.8049096779838718, "
						+ "10.9238795325112861 0.6173165676349102, 10.8314696123025449 0.4444297669803978, 10.7071067811865479 0.2928932188134525, "
						+ "10.5555702330196031 0.1685303876974548, 10.3826834323650896 0.0761204674887133, 10.1950903220161280 0.0192147195967696, "
						+ "10.0000000000000000 0.0000000000000000, 1.0000000000000000 0.0000000000000000))",
				p.buffer(1).toString());
		assertEquals(
				"POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 "
						+ "10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, "
						+ "1.0000000000000000 1.0000000000000000))", p
						.convexHull().toString());
		assertEquals(
				"POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 "
						+ "10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, "
						+ "1.0000000000000000 1.0000000000000000))", p
						.intersection(geom).toString());
		assertEquals(
				"POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 "
						+ "10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, "
						+ "1.0000000000000000 1.0000000000000000))",
				p.Union(geom).toString());
		assertEquals("GEOMETRYCOLLECTION EMPTY", p.difference(geom).toString());
		assertEquals("GEOMETRYCOLLECTION EMPTY", p.symDifference(geom)
				.toString());

	}

}
