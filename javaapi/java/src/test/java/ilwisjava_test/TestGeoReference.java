package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;


public class TestGeoReference {
	private final static String workingDir = TestUtil.workingDir;
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}
	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
        Engine.setWorkingCatalog(workingDir+"raster/");
        ilwisobjects.connectIssueLogger();
	}
	
	@Test
	public void standaloneFile() {
		GeoReference gr = new GeoReference("n000302.grf");
	    assertTrue(gr.isValid());
	    assertFalse(gr.isInternal());
	    assertFalse(gr.centerOfPixel());
	}
	
	@Test
	public void fromRaster() {
		RasterCoverage r = new RasterCoverage("n000302.mpr");
		GeoReference gr = r.geoReference();
        assertTrue(gr.isValid());
        assertFalse(gr.isInternal());
        assertTrue(r.coordinateSystem().equals( gr.coordinateSystem() ));
        assertTrue(r.size().equals( gr.size() ));
        Box box = new Box(gr.size());
        Envelope env = gr.box2Envelope(box);
        assertEquals("0 0 0 1151 1151 0", box.toString());
        assertEquals("-4.61199e+06 -4.60397e+06 4.60401e+06 4.61203e+06", env.toString());
        Envelope subenv = new Envelope(new Coordinate(-1e+06, -1e+06), new Coordinate(1e+06, 1e+06));
        Box subbox = gr.envelope2Box(subenv);
        assertEquals("-1e+06 -1e+06 1e+06 1e+06", subenv.toString());
        assertEquals("451 451 701 701", subbox.toString());
        assertEquals("pixel(536.599,478.436)", gr.coord2Pixel(new Coordinate(-319195.47, 784540.64)).toString());
        assertEquals("coordinate(-319198.248000,784544.506500)", gr.pixel2Coord(new PixelD(536.599, 478.436)).toString());
        assertEquals("coordinate(-323990.248000,788032.506500)", gr.pixel2Coord(new Pixel(536, 478)).toString());
//        assertEquals(0, gr.pixelSize(), TestUtil.precision);  // TODO possible bug in GeoRefImplementaion  - nan is not a good pixelSize!!
        assertTrue(gr.centerOfPixel());
        assertEquals("536 478 536 478",
        		gr.envelope2Box(new Envelope(new Coordinate(-319195.47, 784540.64), new Coordinate(-319990.248000, 784032.506500))).toString());
        gr.setCenterOfPixel(false);
        assertFalse(gr.centerOfPixel());
        assertEquals("536 478 536 478",
                gr.envelope2Box(new Envelope(new Coordinate(-319195.47, 784540.64), new Coordinate(-319990.248000, 784032.506500))).toString());
        gr.setCenterOfPixel(true);
        assertTrue(gr.centerOfPixel()); 
	}

}
