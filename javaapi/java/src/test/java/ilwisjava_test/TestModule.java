package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestModule {
	private final static String workingDir = TestUtil.workingDir;
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}
	
	/*@Test //just logs ilwisExceptions
	public void issueLogger() {
		ilwisobjects.disconnectIssueLogger();
		boolean except = false;
		try {
			FeatureCoverage fc = new FeatureCoverage(workingDir + "/noneexistentDir/nonexistent.file");
		} catch(Exception e) {
			assertEquals("Cann't find suitable factory for nonexistent.file", e.getMessage());
			except = true;
		}
		assertTrue(except);
		ilwisobjects.connectIssueLogger();
	}*/
	
	@Test
	public void ilwisTypes() {
		assertEquals(7, it.ilwisType.FEATURE.swigValue());
		assertTrue( (it.ilwisType.FEATURE.swigValue() & 4) != 0 );
		assertFalse( (it.ilwisType.CATALOG.swigValue()&it.ilwisType.FEATURE.swigValue()) != 0 );
		assertEquals(4, (it.ilwisType.FEATURE.swigValue() & 4));
		assertEquals(524288, it.ilwisType.CATALOG.swigValue());
		//it.ilwisType.CATALOG.setValue(0); //not implemented
		//assertEquals(0, it.ilwisType.CATALOG.swigValue());
		assertEquals(0, it.ilwisType.CONTINUOUSCOLOR.swigValue());
		
	}

}
