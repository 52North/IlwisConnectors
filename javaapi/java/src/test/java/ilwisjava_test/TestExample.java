package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestExample {
	private final static String workingDir = TestUtil.workingDir;
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}
	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
        Engine.setWorkingCatalog(workingDir+"example/");
        ilwisobjects.connectIssueLogger();
	}
	
	@Test
	public void claudio() {
		FeatureCoverage distribution = new FeatureCoverage(workingDir + "example/freq.mpp");
		FeatureCoverage polygongrid = FeatureCoverage.toFeatureCoverage( 
				Engine._do("gridding_1", "gridding", distribution.coordinateSystem().toString(),
						new Coordinate(26.5, 4.5).toString(),"1", "1", "15", "13")
				);
		assertTrue( polygongrid.name().matches("gridding_[0-9]*") );
		polygongrid.addColumn("maxY", "value");
		double maxval = 0;
		for(Feature polygon : polygongrid ) {
			for(Feature point : distribution) {
				if (polygon.geometry().contains( point.geometry() )) {
					if(polygon.attribute("maxY", 0.0) != ilwisobjects.getRUNDEF()) {
						maxval = Math.max(polygon.attribute("maxY", 0.0), point.attribute("freq_speciesY", 0.0));
					} else {
						maxval = 0;
					}
					polygon.setAttribute("maxY", maxval);
				}
			}
		}
		polygongrid.store(workingDir + "example/polygongrid", "vectormap", "ilwis3");
	}

}
