package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestThematicDomain {
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}
	
	@Test
	public void containement() {
		ThematicRange tr = new ThematicRange();
		tr.add("hound", "3.1", "Fierce doggy");
        tr.add("greyhound", "0.32", "the fast one");

        assertEquals("hound", tr.listAll().get(0).get(0));
        assertEquals("3.1", tr.listAll().get(0).get(1));
        assertEquals("Fierce doggy", tr.listAll().get(0).get(2));
        assertEquals("the fast one", tr.listAll().get(1).get(2));

        ItemDomain td = new ItemDomain(tr);

        tr.add("foxhound", "2.4", "hunting foxes");

        ItemDomain td2 = new ItemDomain(tr);
        td.setParent(td2);
        td.setStrict(false);

        assertEquals(td.contains("hound"), "cSELF");
        assertEquals(td.contains("greyhound"), "cSELF");
        assertEquals(td.contains("foxhound"), "cPARENT");
        assertEquals(td.contains("ghosthound"), "cNONE");
	}
	
	@Test(expected=Exception.class)
	public void parents() {
		ThematicRange tr = new ThematicRange();
		tr.add("hound", "3.1", "Fierce doggy");
        tr.add("greyhound", "0.32", "the fast one");
        
        ItemDomain td = new ItemDomain(tr);
        
        td.parent(); //raises exception
        
        tr.add("foxhound", "2.4", "hunting foxes");
        ItemDomain td2 = new ItemDomain(tr);
        td.setParent(td2);
        assertTrue(td.parent().isValid());
	}
	
	@Test
	public void removeAndCount() {
		ThematicRange tr = new ThematicRange();
	    tr.add("hound", "3.1", "Fierce doggy");
	    tr.add("greyhound", "0.32", "the fast one");

	    ItemDomain td = new ItemDomain(tr);
	    assertEquals(2, td.count());
	    td.removeItem("hound");
	    assertEquals(1, td.count());
	    //td.addItem("fox", "34", "wait a second"); //ItemDomain.addItem not implemented
	    //assertEquals(2, td.count());
	}
	
	@Test
	public void theme() {
		ThematicRange tr = new ThematicRange();
		tr.add("hound", "3.1", "Fierce doggy");
	    tr.add("greyhound", "0.32", "the fast one");
	    ItemDomain td = new ItemDomain(tr);

	    td.setTheme("Hounds");
	    assertEquals("Hounds", td.theme());
	}
	
	
}
