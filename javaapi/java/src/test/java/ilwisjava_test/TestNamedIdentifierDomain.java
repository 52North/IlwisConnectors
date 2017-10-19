package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestNamedIdentifierDomain {
	private final static String workingDir = ilwisjava_test.TestUtil.workingDir;

	@BeforeClass
	public static void onceExecutedBeforeAll() {
		ilwisjava_test.TestUtil.onceExecutedBeforeAll();
	}

	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
		Engine.setWorkingCatalog(workingDir + "raster/");
		ilwisobjects.connectIssueLogger();
	}

	@Test
	public void TestContainment() {
		NamedItemRange nr = new NamedItemRange();
		nr.add("Perth");
		nr.add("Darwin");

		assertEquals("Perth", nr.listAll().get(0));
		assertEquals("Darwin", nr.listAll().get(1));

		ItemDomain nchild = new ItemDomain(nr);

		nr.add("Broome");
		ItemDomain nparent = new ItemDomain(nr);

		nchild.setParent(nparent);
		nchild.setStrict(false);

		assertEquals(nchild.contains("Perth"), "cSELF");
		assertEquals(nchild.contains("Darwin"), "cSELF");
		assertEquals(nchild.contains("Broome"), "cPARENT");
		assertEquals(nchild.contains("Adelaide"), "cNONE");

		nchild.setStrict(true);
		assertEquals(nchild.contains("Broome"), "cNONE");
	}

	@Test(expected = Exception.class)
	public void test_parents() {

		NamedItemRange nr = new NamedItemRange();
		nr.add("Perth");
		nr.add("Darwin");

		ItemDomain nchild = new ItemDomain(nr);

		nchild.parent(); // drops exception

		nr.add("Broome");
		ItemDomain nparent = new ItemDomain(nr);
		nchild.setParent(nparent);
		assertTrue(nchild.parent().isValid());
	}

	@Test
	public void removeAndCount() {
		NamedItemRange nr = new NamedItemRange();
		nr.add("Perth");
		nr.add("Darwin");

		ItemDomain namedDom = new ItemDomain(nr);
		assertEquals(namedDom.count(), 2);
		namedDom.removeItem("Perth");
		assertEquals(namedDom.count(), 1);
		// namedDom.addItem("Childers"); //ItemDomain.addItem not implemented
		// assertEquals(namedDom.count(), 2);
	}

	@Test
	public void theme() {
		NamedItemRange nr = new NamedItemRange();
		nr.add("Perth");
		nr.add("Darwin");

		ItemDomain namedRange = new ItemDomain(nr);

		namedRange.setTheme("Australian Cities");
		assertEquals(namedRange.theme(), "Australian Cities");
	}
}
