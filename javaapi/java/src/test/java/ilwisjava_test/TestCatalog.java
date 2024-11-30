package ilwisjava_test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestCatalog {
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
	
	@SuppressWarnings("unused")
	@Test
	public void fromGPX() {
		Catalog cat = new Catalog(workingDir + "feature/test.gpx");
		String[] catItems = {"route_points", "route_points", "routes", "routes", "track_points", "track_points",
                "tracks", "tracks", "waypoints", "waypoints"};
		for(int i=0;i<catItems.length;++i)
			assertEquals(catItems[i], cat.items().get(i));
	    assertTrue(cat.isValid());
	    FeatureCoverage trks = FeatureCoverage.toFeatureCoverage( cat._getitem("tracks") );
	    assertTrue(trks.isValid());
		FeatureIterator it = trks.iterator();
	    assertEquals(1, trks.featureCount());
	    
	    FeatureCoverage trkpts = FeatureCoverage.toFeatureCoverage( cat._getitem("track_points") );
	    assertTrue(trkpts.isValid());
	    it = trkpts.iterator();
	    assertEquals(830, trkpts.featureCount());
	}
	
}
