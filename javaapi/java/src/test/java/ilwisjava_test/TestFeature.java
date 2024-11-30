package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestFeature {
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
	public void featureCoverage() {
		FeatureCoverage fc = new FeatureCoverage("rainfall.shp");
	    assertTrue("FeatureCoverage(rainfall.shp) not loaded correctly!", fc.isValid());
	    assertEquals("internal FeatureCoverage name wrong!", fc.name(), "rainfall.shp");
	    assertEquals("feature count wrong", 13, fc.featureCount());
	    assertTrue("FeatureCoverage.addAttribute failed!", fc.addColumn("sum", "value"));
	    
	    Geometry g = new Geometry("POINT(5.4 6 9.0)", fc.coordinateSystem());
	    Feature newfeature = fc.newFeature(g);
	    Feature newfeature2 = fc.newFeature("POINT(5.4 6 9.0)", fc.coordinateSystem());
	    assertTrue("newfeature creation failed!", newfeature.isValid());
	    
	    assertEquals("new feature count wrong", 15, fc.featureCount());
	    
	    FeatureCoverage fc_invalid = new FeatureCoverage("newFC");
	    //assertTrue(fc_invalid.isInternal());
	    //assertFalse(fc_invalid.isValid());
	    g = new Geometry("POINT(5.4 6 9.0)", new CoordinateSystem("code=epsg:23035"));
	    /*boolean except = false; //doesn't drop exception, just fails
	    try {
	    	newfeature = fc_invalid.newFeature(g);
	    } catch (Exception e) {
	    	except = true;
	    	System.out.println(e.getMessage());
	    }
	    assertTrue(except);*/  
	}
	
	@Test
	public void featureIterator() {
		FeatureCoverage fc = new FeatureCoverage("rainfall.shp");
	    assertTrue("FeatureCoverage(rainfall.shp) not loaded correctly!", fc.isValid());
	    double summ = 0;
	    for(Feature f : fc) {
	    	double tempsum = f.attribute("MAY", 0.0);
	    	if (tempsum != ilwisobjects.getRUNDEF())
	    		summ += (int)tempsum;
	    	f.setAttribute("sum", Double.toString(summ));
	    	assertTrue("wrong feature representation", f.toString().matches("Feature\\([0-9]*\\)") );	
	    	assertTrue("wrong geometry representation", f.geometry().toString()
	    			.matches("POINT\\s\\(([0-9\\.\\-]+|\\-1e\\+308|5\\.4)\\s([0-9\\.\\-]+|\\-1e\\+308|[0-9]+\\.[0-9]+e\\+[0-9]+)\\)") );
	    }
	    if (fc.featureCount() == 14)
	    	assertEquals("wrong sum over rainfall in MAY!", 298.0, summ, TestUtil.precision);
	    else
	    	assertEquals("wrong sum over rainfall in MAY!", 286.0, summ, TestUtil.precision);
	    
	    FeatureIterator it = new FeatureIterator(fc);
	    assertTrue(fc.isValid());
	    assertEquals("FeatureIterator for rainfall.shp", it.toString());
	    FeatureIterator it2 = it.add(2);
	    assertTrue( it.notequal(it2) );
	    assertFalse( it.equals(it2) );
	    FeatureIterator it3 = it2.subtract(2);
	    assertTrue( it.equals(it3) );
	    
	    Feature f = it3.current();
	    int index = Integer.parseInt( f.toString().substring(8, f.toString().length()-1) );
	    f = it3.next();
	    assertEquals("Feature(" + index + ")", f.toString());
	    f = it3.next();
	    assertEquals("Feature(" + (index+1) + ")", f.toString());
	}
	
	@Test
	public void featureAttributes() {
		FeatureCoverage fc = new FeatureCoverage("GDAL_OGR_feature.vrt");
	    assertEquals("meta data contains wrong featureCount", 100, fc.featureCount());
	    FeatureIterator it = fc.iterator();
	    Feature f = it.next();
	    assertEquals("after loading binary data featureCount is correct", 3, fc.featureCount());
	    assertEquals("unsuccessfully altered geometry",
	    		"LINESTRING (1.0000000000000000 1.0000000000000000, 2.0000000000000000 2.0000000000000000)", f.geometry().toWKT());
	    f.geometry().fromWKT("POINT(5.4 6 9.0)");
	    assertEquals("not typecheck! butunsuccessfully altered geometry",
	    		"POINT (5.4000000000000004 6.0000000000000000)", f.geometry().toWKT() );
	    boolean except = false;
	    try {
	    	f.attribute("wrongColum", "");
	    } catch (Exception e) {
	    	except = true;
	    	assertEquals("No attribute 'wrongColum' found", e.getMessage());
	    }
	    assertTrue(except);
	    String v = f.attribute("String", "");
	    assertEquals("wrong attribute value", "LINESTRING(1 1, 2 2)", v);
	    f.setAttribute("String", 12);
	    assertEquals("no real type check here since it could be converted back and forth", 12, f.attribute("String", 0) );
	    assertEquals(4123045, f.attribute("Integer", 0));
	    f.setAttribute("Integer", (long)-1e+15); // MIN(qlonglong)
	    assertEquals((long)-1e+15, f.attribute("Integer", 0));
	    f.setAttribute("Integer", (long)1e+15); // MAX(qlonglong)
	    assertEquals((long)1e+15, f.attribute("Integer", 0));
	    //overflow tests can not be done with java long
	    
	    f.setAttribute("Float", "9223372036854775808");
	    assertEquals("9223372036854775808", f.attribute("Float", ""));
	    f.setAttribute("Float", 9223372036854775808.);
	    assertEquals(9223372036854775808., f.attribute("Float", 0.), TestUtil.precision);
	    f.setAttribute("Float", 0.432);
	    assertEquals(0.432, f.attribute("Float", 0.), TestUtil.precision);
	    f.setAttribute("Float", 23.4e-32);
	    assertEquals(23.4e-32, f.attribute("Float", 0.), TestUtil.precision);
	    
	    //TODO datetime tests
	}

	@Test
	public void loadGDALstorageGDAL() {
		FeatureCoverage world;
		// polygons
        world = new FeatureCoverage("ne_110m_admin_0_countries.shp");
        assertTrue(world.isValid());
        assertFalse(world.isInternal());
        world.store(workingDir + "temp/countries_fromshp.shp", "ESRI Shapefile", "gdal");
        // points
        world = new FeatureCoverage("rainfall.shp");
        assertTrue(world.isValid());
        assertFalse(world.isInternal());
        world.store(workingDir + "temp/rainfall_fromshp.shp", "ESRI Shapefile", "gdal");
        // lines
        world = new FeatureCoverage("drainage.shp");
        assertTrue(world.isValid());
        assertFalse(world.isInternal());
        world.store(workingDir + "temp/drainage_fromshp.shp", "ESRI Shapefile", "gdal");
	}
	
	@Test
	public void loadGDALstorageIlwis3() {
		// polygons
        FeatureCoverage world = new FeatureCoverage("ne_110m_admin_0_countries.shp");
        assertTrue(world.isValid());
        assertFalse(world.isInternal());
        // points
        world = new FeatureCoverage("rainfall.shp");
        assertTrue(world.isValid());
        assertFalse(world.isInternal());
//        world.setCoordinateSystem(new CoordinateSystem(workingDir + "feature/countries.csy"));
        world.store(workingDir + "temp/rainfall_fromshp", "vectormap", "ilwis3");
	}
}
