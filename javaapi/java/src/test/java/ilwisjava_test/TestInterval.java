package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestInterval {
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}
	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
        ilwisobjects.connectIssueLogger();
	}
	
	@Test
	public void containment() {
		NumericItemRange interrange = new NumericItemRange();
		interrange.add("sealevel", 40, 100);
		interrange.add("dijks", 101, 151);

		assertEquals("sealevel", interrange.listAll().get(0).get(0));
		assertEquals("numericrange:40.0|100.0", interrange.listAll().get(0)
				.get(1));
		assertEquals("dijks", interrange.listAll().get(1).get(0));
		assertEquals("numericrange:101.0|151.0", interrange.listAll().get(1)
				.get(1));

		ItemDomain childdom = new ItemDomain(interrange);

		interrange.add("by the sea", 152.0, 181.0, 5.0);
		ItemDomain parentdom = new ItemDomain(interrange);

		childdom.setParent(parentdom);

		assertEquals("cSELF", childdom.contains(40));
		assertEquals("cNONE", childdom.contains(39.9));
		assertEquals("cNONE", childdom.contains(161.1));

		childdom.setStrict(false);
		assertEquals("cPARENT", childdom.contains(161.1));
	}

	@Test
	public void parents() {
		NumericItemRange nir = new NumericItemRange();
		nir.add("sealevel", 40, 100);
		ItemDomain childdom = new ItemDomain(nir);

		nir.add("by the sea", 151, 181, 5);
		ItemDomain parentdom = new ItemDomain(nir);

		boolean exception = false;
		try {
			childdom.parent(); // raises exception
		} catch (Exception e) {
			assertEquals("ILWIS ErrorObject: No parent domain found",
					e.getMessage());
			exception = true;
		}
		assertTrue(exception);

		childdom.setParent(parentdom);
		assertTrue(childdom.parent().isValid());
	}

	@Test
	public void removeAndCount() {
		NumericItemRange nir = new NumericItemRange();
		nir.add("sealevel", 40.0, 100.0, 5.0);
		nir.add("by the sea", 151.0, 181.0, 5.0);
		ItemDomain childdom = new ItemDomain(nir);

		assertEquals(2, childdom.count());
		childdom.removeItem("sealevel");
		assertEquals(1, childdom.count());
		//childdom.add("sealevel", 185.0, 250.0, 5.0); //ItemDomain.addItem not implemented
		//assertEquals(2, childdom.count());
	}
	
	@Test
	public void theme() {
		NumericItemRange nir = new NumericItemRange();
		nir.add("sealevel", 40.0, 100.0, 5.0);
		nir.add("by the sea", 151.0, 181.0, 5.0);
	    ItemDomain childdom = new ItemDomain(nir);

	    childdom.setTheme("Seastuff");
	    assertEquals(childdom.theme(), "Seastuff");
	}

}
