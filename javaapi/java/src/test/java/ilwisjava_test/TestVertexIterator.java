package ilwisjava_test;

import static org.junit.Assert.*;

import java.util.ArrayList;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestVertexIterator {
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
	public void vertexIterator() {
		FeatureCoverage fc = new FeatureCoverage(workingDir+"feature/drainage.shp");
		
		for(Feature feat : fc) {
			VertexIterator baseIt = new VertexIterator(feat.geometry());
            VertexIterator beginIt = baseIt.begin();
            VertexIterator endIt = baseIt.end();

            assertTrue( beginIt.compareTo(endIt) < 0 );

            ArrayList<Coordinate> listA = new ArrayList<Coordinate>();
            while (beginIt.compareTo(endIt) != 0) {
            	listA.add( beginIt.current() );
            	beginIt.add(1);
            }
            
            assertTrue( beginIt.compareTo(endIt) == 0 );
            
            ArrayList<Coordinate> listB = new ArrayList<Coordinate>();
            for (Coordinate c : feat.geometry()) {
            	listB.add(c);
            }
            
            assertTrue(listA.size() > 0);
            assertEquals(listA.size(), listB.size());
            
            for(int i=0;i<listA.size();++i) {
            	assertTrue(listA.get(0).equals(listB.get(0)));
            }
		}
	}

}
