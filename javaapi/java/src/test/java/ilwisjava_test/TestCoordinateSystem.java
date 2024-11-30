package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;


public class TestCoordinateSystem {
	private final static String workingDir = TestUtil.workingDir;

	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}

	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
        Engine.setWorkingCatalog(workingDir+"feature/");
        ilwisobjects.connectIssueLogger();
	}
	
	@Test
	public void fromFile() {
		CoordinateSystem csy = new CoordinateSystem(workingDir + "feature/cochabamba.csy");
	    assertEquals("cochabamba.csy", csy.name());
	    FeatureCoverage fc = new FeatureCoverage("Rainfall.mpp");
	    CoordinateSystem rainCsy = fc.coordinateSystem();
	    assertEquals(csy.ilwisID(), rainCsy.ilwisID());
	    assertEquals(csy.envelope().toString(), rainCsy.envelope().toString());
	}
	
	@Test
	public void proj4() {
		CoordinateSystem cs1 = new CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs");
            cs1.name("myProj4CSY");
            assertTrue("invalid CoordinateSystem from Proj4", cs1.isValid());
            FeatureCoverage fc = new FeatureCoverage(workingDir + "feature/rainfall.shp");
            fc.setCoordinateSystem(cs1);
            assertEquals("could not alter FeatureCoverage's CoordinateSystem",
            		fc.coordinateSystem().name(), "myProj4CSY" );
            assertEquals(cs1.toProj4(),
                             "+proj=utm +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +zone=35");
	}

	@Test
	public void EPSG() {
		CoordinateSystem cs2 = new CoordinateSystem("code=epsg:23035");
	    assertTrue(cs2.isValid());
	    assertEquals("ED50 / UTM zone 35N", cs2.name());
	}
	
	@Test
	public void isEqual() {
		CoordinateSystem cs1 = new CoordinateSystem(
				"code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs");
		CoordinateSystem cs2 = new CoordinateSystem("code=epsg:23035");
	    assertTrue( cs1.equals(cs2) );
	    assertFalse( cs1.notequal(cs2) );
	}
	
	@Test
	public void envelope() {
		ilwisobjects.disconnectIssueLogger();
        Engine.setWorkingCatalog(workingDir+"raster/");
        ilwisobjects.connectIssueLogger();
		//RasterCoverage rc = new RasterCoverage(workingDir + "raster/n000302.mpr"); //fails
        RasterCoverage rc = new RasterCoverage(workingDir + "raster/small.mpr");
	    Envelope env = rc.envelope();
	    CoordinateSystem csyNew = new CoordinateSystem("code=epsg:2050");
	    Envelope newEnv = rc.coordinateSystem().convertEnvelope(csyNew, env);
	    assertNotEquals(env.toString(), newEnv.toString());
	}
}
