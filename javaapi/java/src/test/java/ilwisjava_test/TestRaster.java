package ilwisjava_test;

import static org.junit.Assert.*;

import java.math.BigInteger;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestRaster {
	private final static String workingDir = TestUtil.workingDir;
	private static double[] small;

	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
		small = new double[] { 
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
		};
	}

	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
		Engine.setWorkingCatalog(workingDir + "raster/");
		ilwisobjects.connectIssueLogger();
	}

	@Test
	public void rasterCalculation() {
		RasterCoverage rc = new RasterCoverage(workingDir+"raster/n000302.mpr");
		RasterCoverage rctif = new RasterCoverage(workingDir+"raster/n0.mpr");
		assertTrue("couldn't load RasterCoverages", rc.isValid() && rctif.isValid() );
	}


	@Test
	public void reprojectRaster() {
		RasterCoverage rc = new RasterCoverage("n000302.mpr");
		CoordinateSystem targetCsy = new CoordinateSystem("code=epsg:2050");
		GeoReference sourceGeoref = rc.geoReference();
		CoordinateSystem sourceCsy = rc.coordinateSystem();
		Envelope sourceEnv = rc.envelope();
		Envelope targetEnv = sourceCsy.convertEnvelope(targetCsy, sourceEnv);
	  
		Box bo = sourceGeoref.envelope2Box(targetEnv);
		Size sz = bo.size();
	  
		String newGeoRefStr = "code=georef:type=corners,csy=epsg:2050,envelope="
				+ targetEnv + ",gridsize=" + sz.xsize() + " " + sz.ysize() + ",name=grf1";
	  
		GeoReference targetGeoRef = new GeoReference(newGeoRefStr);
		IObject rcReproj = Engine._do("resample_1", "resample", rc.name(), targetGeoRef.name(), "bilinear");
		assertTrue(rcReproj.isValid());
	  
		RasterCoverage rcReproj2 = rc.reprojectRaster("newraster", 2050,
				"bilinear"); assertTrue(rcReproj2.isValid());
	}
	 

	@Test
	public void rasterSelectionWKT() {
		RasterCoverage rc = new RasterCoverage("n000302__1.mpr");
		RasterCoverage rcSelWKT = rc.select(
				"Polygon((495209 80832,927209 -999367, 1887209 -1282307,2184809 311232,495209 80832))");
		assertTrue(rcSelWKT.isValid());
		Pixel pix1 = new Pixel(740, 650, 0);
		Pixel pix2 = new Pixel(300, 400, 0);
		Coordinate coord = rc.geoReference().pixel2Coord(pix1);
		PixelD pixSel = rcSelWKT.geoReference().coord2Pixel(coord);
		  
		pixSel.setY( (int)(pixSel.y())+1 );
		  
		assertEquals(rcSelWKT.pix2value(pixSel), rc.pix2value(pix1), TestUtil.precision);
		assertNotEquals(rcSelWKT.pix2value(pix2), rc.pix2value(pix2));
		
		rcSelWKT.store(workingDir + "raster/aa_select_n000302", "map", "ilwis3"); 
	}

	@Test
	public void pixelIterator() {
		RasterCoverage rcl = new RasterCoverage("small.mpl");
		assertTrue("couldn't load small.mpl", rcl.isValid());
		assertEquals("small.mpl", rcl.name());

		int j = 0;
		for (Double v : rcl) {
			assertEquals(v, small[j], TestUtil.precision);
			++j;
		}

		PixelIterator it = rcl.iterator();
		assertEquals("wrong string representation for PixelIterator",
				"PixelIterator for small.mpl at position pixel(0,0,0)",
				it.toString());
		assertTrue("couldn't create PixelIterator from small.mpl", it.isValid());
		BigInteger lin = rcl.size().linearSize();
		assertEquals("wrong linear size of small.mpl", BigInteger.valueOf(175),
				lin);
		for (int i = 0; i < lin.intValue(); ++i) {
			if (it.yChanged())
				assertTrue("yChanged not only every 5th step (i=" + i + ")",
						i % 5 == 0);
			else
				assertFalse("yChanged not only every 2nd step (i=" + i + ")",
						(i % 5 == 0) && (i != 0));

			if (it.zChanged())
				assertTrue("zChanged not only every 25th step (i=" + i + ")",
						i % 25 == 0);
			else
				assertFalse("zChanged not only every 2nd step (i=" + i + ")",
						(i % 25 == 0) && (i != 0));
			assertEquals(small[i], it.next().doubleValue(), TestUtil.precision);
		}

		PixelIterator rit = rcl.iterator();
		assertTrue(rit.isValid());
		assertEquals("pixel(0,0,0)", rit.position().toString());
		rit.increase(4);
		assertEquals("pixel(4,0,0)", rit.position().toString());
		PixelIterator rit2 = rit.add(3);
		assertEquals("pixel(2,1,0)", rit2.position().toString());
		PixelIterator rit3 = rit.add(13);
		assertEquals("pixel(2,3,0)", rit3.position().toString());
		assertEquals("pixel(4,0,0)", rit.position().toString());
		assertTrue(rit.contains(new Pixel(1, 1, 1)));
		assertFalse(rit.contains(new Pixel(5, 1, 1)));
		assertNotEquals(0, rit3.compareTo(rit2));
		assertEquals(22.0, rit2.toDouble(), TestUtil.precision);
		assertEquals(120.0, rit.get(23), TestUtil.precision);
		assertEquals(120.0, rit.toDouble(), TestUtil.precision);
		rit.set(23, 434);
		assertEquals(434, rit.get(23), TestUtil.precision);
		rit2 = new PixelIterator(rit.get(new Pixel(2, 2, 2)));
		assertEquals("pixel(2,2,2)", rit.position().toString());
		assertEquals(62, rit.toBigInteger().intValue());
		assertEquals(10.0, rit.next(), TestUtil.precision);
		assertTrue(rit.xChanged());
		assertFalse(rit.yChanged());
		assertFalse(rit.zChanged());
		assertEquals("pixel(3,2,2)", rit.position().toString());
		assertEquals("pixel(2,2,2)", rit2.position().toString());
		assertNotEquals(0, rit.compareTo(rit2));
		assertEquals(1, rit.compareTo(rit2));
		assertEquals(63, rit.toBigInteger().intValue());
		assertEquals(101.0, rit.toDouble(), TestUtil.precision);
		Box box = new Box("(1 1 0,2 2 3)");
		assertEquals("1 1 0 2 2 3", box.toString());
		PixelIterator bit = new PixelIterator(rcl, box);
		assertTrue(bit.isValid());
		String[] boxed0 = new String[] { "pixel(1,1,0)", "pixel(2,1,0)", "pixel(1,2,0)",
				"pixel(2,2,0)", "pixel(1,1,1)", "pixel(2,1,1)", "pixel(1,2,1)", 
				"pixel(2,2,1)", "pixel(1,1,2)", "pixel(2,1,2)", "pixel(1,2,2)",
				"pixel(2,2,2)", "pixel(1,1,3)", "pixel(2,1,3)", "pixel(1,2,3)",
				"pixel(2,2,3)" };
		double[] boxed1 = new double[] { 22.0, 22.0, 132.0, 35.0, 32.0, 73.0,
				78.0, 24.0, 60.0, 15.0, 12.0, 10.0, 174.0, 174.0, 78.0, 174.0
		};
		assertTrue( bit.contains(new Pixel(1, 1, 1)) );
		assertFalse( bit.contains(new Pixel(0, 0, 0)) );
		
		int lin1 = bit.box().size().linearSize().intValue();
		assertEquals(16, lin1);
		for(int i=0; i<lin1; ++i) {
			assertEquals(boxed0[i], bit.position().toString());
			if (bit.yChanged())
                assertTrue("yChanged not only every 2nd step (i=" + i + ")", 
                		i % 2 == 0 );
            else
                assertFalse("yChanged not only every 2nd step (i=" + i + ")",
                		(i % 2 == 0) && (i != 0) );
            if (bit.zChanged())
                assertTrue("zChanged not only every 4th step (i=" + i + ")", 
                		i % 4 == 0 );
            else
                assertFalse("zChanged not only every 4th step (i=" + i + ")",
                		(i % 4 == 0) && (i != 0));			
			assertEquals(boxed1[i], bit.next().doubleValue(), TestUtil.precision);
		}	
	}
	
	@Test
	public void band() {
		RasterCoverage rc = new RasterCoverage("landuse.mpr");
        PixelIterator pixIt = rc.band(0);
        assertTrue(pixIt.isValid());
	}
	
	//TODO reprojectRaster, datadefinition

}
