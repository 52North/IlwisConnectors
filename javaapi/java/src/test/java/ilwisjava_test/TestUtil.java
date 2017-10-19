package ilwisjava_test;

import static org.junit.Assert.*;

import java.io.File;
import java.io.FileNotFoundException;
import java.math.BigInteger;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestUtil {

	public final static String workingDir = ClassLoader.getSystemResource("").getPath().toString() + "testfiles/";
	public static String ilwisLocation;
	public final static double precision = 1e-10;

	@BeforeClass
	public static void onceExecutedBeforeAll() {
		try {
			ilwisobjects.initIlwisObjects();
		} catch (FileNotFoundException e) {
			fail(e.getMessage() );
		} 
		ilwisLocation = ilwisobjects.getIlwisLocation();
		
		// Checks if the test resources (raster folder) available
		File f = new File(workingDir + "raster");
		assertTrue("Test folder not available!", f.isDirectory());
	}

	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
		ilwisobjects.connectIssueLogger();
	}

	@Test
	public void TestPixelCoordinate() {
		Pixel p = new Pixel(4, 5);
		assertTrue(p.isValid());
		assertEquals("pixel(4,5)", p.toString());
		p.multiply(3);
		assertTrue(p.equals(new Pixel(12, 15)));
		p.divide(4);
		assertEquals("pixel(3,3)", p.toString());
		assertTrue(p.notequal(new Pixel(3, 4)));
		assertFalse(p.is3D());

		Pixel v = new Pixel(4, 5, 6);
		assertEquals("pixel(4,5,6)", v.toString());
		assertTrue(v.is3D());
		assertEquals(4, v.x());
		v.setX(32);
		assertEquals(32, v.x());
		v.setY(32);
		assertEquals(32, v.y());
		v.setZ(32);
		assertEquals(32, v.z());
		assertEquals("pixel(32,32,32)", v.toString());

		Coordinate c = new Coordinate(-1871900.47, 1874237.55);
		assertEquals(true, c.isValid());
		assertEquals(false, c.is3D());
		assertEquals("coordinate(-1871900.470000,1874237.550000)", c.toString());
		c.multiply(6);
		assertEquals(true,
				c.equals(new Coordinate(-11231402.820000, 11245425.300000)));
		assertEquals("coordinate(-11231402.820000,11245425.300000)",
				c.toString());
		c.divide(6);
		assertEquals("coordinate(-1871900.470000,1874237.550000)", c.toString());
		assertEquals(false, c.is3D());
		c = new Coordinate(-1871900.47, 1874237.55, 2);
		assertEquals(true, c.is3D());
		assertEquals("coordinate(-1871900.470000,1874237.550000,2.000000)",
				c.toString());
		assertEquals(-1871900.47, c.x(), TestUtil.precision);
		c.setX(4.323);
		assertEquals(4.323, c.x(), TestUtil.precision);
	}

	@Test
	public void sizeSizeD() {
		Size sz = new Size(2, 4, 5);
		assertTrue(sz.isValid());
		assertEquals(sz.toString(), "Size(2, 4, 5)");
		assertEquals(sz.linearSize(), BigInteger.valueOf(40));
		sz.multiply(2);
		assertEquals(sz.toString(), "Size(4, 8, 10)");
		sz.increase(new Size(4, 4, 0));
		assertTrue(sz.equals(new Size(8, 12, 10)));
		sz.decrease(new Size(1, 1, 0));
		assertTrue(sz.equals(new Size(7, 11, 10)));
		sz.setXsize(3);
		assertEquals(sz.xsize(), 3);
		assertTrue(sz.contains(new Pixel(1, 1, 1)));
		assertFalse(sz.contains(new Pixel(5, 5, 5)));

		SizeD szd = new SizeD(sz);
		assertEquals(szd.toString(), "Size(3, 11, 10)");
		assertTrue(szd.equals(new SizeD(3., 11., 10.)));
		assertTrue(szd.contains(new Pixel(2, 10, 9)));
		assertTrue(szd.contains(new PixelD(2., 10., 9.)));
		assertTrue(szd.contains(new Coordinate(2., 10., 9.)));
		assertEquals(szd.toString(), "Size(3, 11, 10)");
		szd.decrease(new SizeD(100., 100.)); // == SizeD(100., 100., 1.)
		assertEquals(szd.toString(), "Size(0, 0, 9)");
		assertFalse(szd.isValid());
		assertFalse(new SizeD(10, 10, 0).isValid());
		szd.decrease(new SizeD(10, 10, 0)); // calculation with invalid szd ==
											// Size(0, 0, 9) results in Size(0, 0, 0);
		assertEquals(szd.toString(), "Size(0, 0, 0)");
		szd = new SizeD(2.5, 4.4, 5.1);
		assertTrue(szd.equals(new SizeD(2.5, 4.4, 5.1)));
		assertEquals(szd.toString(), "Size(2.5, 4.4, 5.1)");
		assertEquals(szd.linearSize(), BigInteger.valueOf(40)); // == int(2.5) * int(4.4) * int(5.1)
		szd.multiply(2);
		assertEquals(szd.toString(), "Size(5, 8.8, 10.2)");

		sz = new Size(32, 43);
		assertEquals(sz.toString(), "Size(32, 43, 1)");
	}

	@Test
	public void BoxEnvelope() {
		Box b = new Box(new Pixel(3, 4, 5), new Pixel(4, 5, 6));
		assertTrue(b.isValid());
		assertTrue(b.is3D());
		assertEquals(b.toString(), "3 4 5 4 5 6");
		assertTrue(b.equals(new Box("POLYGON(3 4 5,4 5 6)")));
		assertEquals(b.minCorner().toString(), "pixel(3,4,5)");
		b.minCorner().setX(39);
		assertEquals(b.minCorner().toString(), "pixel(3,4,5)");
		assertTrue(b.minCorner().equals(new Pixel(3, 4, 5)));
		assertEquals(b.maxCorner().toString(), "pixel(4,5,6)");
		assertEquals(b.size().toString(), "Size(2, 2, 2)");
		assertTrue(b.size().equals(new Size(2, 2, 2)));
		assertEquals(b.size().linearSize(), BigInteger.valueOf(8));
		b = new Box(new Pixel(2, 3), new Pixel(4, 5));
		assertEquals(b.toString(), "2 3 4 5");
		assertFalse(b.is3D());
		assertTrue(b.isValid());

		Envelope env = new Envelope(new Coordinate(3.6111119, 4.7, 5.9),
				new Coordinate(4.7, 5.8, 6.9));
		assertEquals(env.toString(), "3.61111 4.7 5.9 4.7 5.8 6.9");
		assertEquals(env.size().toString(), "Size(2.08889, 2.1, 2)");
		env = new Envelope("POLYGON(3.6111119 4.7 5.9,4.7 5.8 6.9)");
		assertEquals(env.toString(), "3.61111 4.7 5.9 4.7 5.8 6.9");
		assertEquals(env.size().toString(), "Size(2.08889, 2.1, 2)");
		assertFalse(env.size().equals(new SizeD(2.08889, 2.1, 2)));
		env = new Envelope(env.size());
		assertEquals("0 0 0 1.08889 1.1 1", env.toString());
		env = new Envelope(new Coordinate(3, 4, 5), new Coordinate(4, 5, 6));
		assertEquals(env.toString(), "3 4 5 4 5 6");
		env = new Envelope(env.size());
		assertEquals(env.toString(), "0 0 0 1 1 1");
		assertTrue(env.contains(new Coordinate(.5, .5, .5)));
		assertTrue(env.contains(env));
	}

}
