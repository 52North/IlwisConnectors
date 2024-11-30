package ilwisjava_test;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.Engine;
import org.n52.ilwis.java.IObject;
import org.n52.ilwis.java.RasterCoverage;
import org.n52.ilwis.java.ilwisobjects;

public class TestBaby {
	private final static String workingDir = TestUtil.workingDir;
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}
	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
        Engine.setWorkingCatalog(workingDir+"baby/");
        ilwisobjects.connectIssueLogger();
	}
	
	@Test
	public void helloRaster() {
		RasterCoverage rc = new RasterCoverage("n000302.mpr");
	    IObject res = Engine._do("aggregateraster_1", "aggregateraster", rc.toString(), "Avg", "50", "true");
	    RasterCoverage result = RasterCoverage.toRasterCoverage(res);
	    result.store(workingDir + "baby/avg_n000302", "map", "ilwis3");
	    
	    rc = new RasterCoverage("n000302.mpr");
	    res = Engine._do("aggregateraster_2", "aggregateraster", rc.toString(), "Avg", "10", "true");
	    result = RasterCoverage.toRasterCoverage(res);
	    result.store(workingDir + "baby/avg_n000302_2", "map", "ilwis3");
	}

}
