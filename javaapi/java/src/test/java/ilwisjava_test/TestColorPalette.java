package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestColorPalette {
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}

	@Test
	public void range() {
		/*try {
			System.out.println("wait for enter");
			System.in.read();
			System.out.println("start");
		} catch(Exception e) {}*/
		Color col1 = new Color(ColorModel.Value.cmRGBA,
				new vectord(new double[] { 220.0, 20.0, 30.0, 200.0 }));
		Color col2 = new Color(ColorModel.Value.cmRGBA,
				new vectord(new double[] { 255.0, 80.0, 60.0, 240.0 }));
		Color col3 = new Color(ColorModel.Value.cmRGBA,
				new vectord(new double[] { 255.0, 80.0, 69.0, 240.0 }));
		ColorPalette colPal = new ColorPalette();

		colPal.add(col1);
		assertEquals(1, colPal.count());
		colPal.add(col2);
		assertEquals(2, colPal.count());
		/*assertEquals("RGBA(0.86,0.08,0.12,0.78)", colPal.item("blue").toString());// !
		
		
		String name1 = colPal.itemByOrder(1).getName();// !
		assertFalse(colPal.containsColor(col3));
		colPal.add(col3);
		assertTrue(colPal.containsColor(col3));
		assertEquals(3, colPal.count());
		colPal.remove(name1);
		assertEquals(2, colPal.count());
		assertEquals("RGBA(1,0.31,0.27,0.94)", colPal.color(1).toString());// !

		colPal.clear();
		assertEquals(colPal.valueAt(1, colPal).toString(), colPal.color(1)
				.toString());*/
	}
}
