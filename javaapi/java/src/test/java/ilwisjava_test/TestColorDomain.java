package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestColorDomain {
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}

	@Test
	public void colorcontainment() {
		Color color1 = new Color(ColorModel.Value.cmRGBA,
				new vectord(new double[] { 220.0, 20.0, 30.0, 200.0 }));
		Color color2 = new Color(ColorModel.Value.cmRGBA,
				new vectord(new double[] { 255.0, 80.0, 60.0, 240.0 }));
		Color color3 = new Color(ColorModel.Value.cmRGBA,
				new vectord(new double[] { 230.0, 60.0, 50.0, 240.0 }));

		ContinuousColorRange col = new ContinuousColorRange(color1, color2);
		assertTrue(col.isValid());

		ContinuousColorRange col2 = col.clone();
		assertTrue(col2.isValid());

		col.defaultColorModel(ColorModel.Value.cmRGBA);
		assertEquals(col.defaultColorModel(), ColorModel.Value.cmRGBA);

		ColorDomain colDom = new ColorDomain("testdomain");
		colDom.setRange(col);
		assertEquals(colDom.containsColor(color3), "cSELF");
	}

	@Test
	public void colorCYMKA() {
		Color color1 = new Color(ColorModel.Value.cmCYMKA,
				new vectord(new double[] { 0.6, 0.2, 0.16, 0.6, 1.0 }));
		Color color2 = new Color(ColorModel.Value.cmCYMKA,
				new vectord(new double[] { 0.9, 0.7, 0.5, 0.9, 1.0 }));
		Color color3 = new Color(ColorModel.Value.cmCYMKA,
				new vectord(new double[] { 0.77, 0.5, 0.4, 0.7, 1.0 }));
		ContinuousColorRange col = new ContinuousColorRange(color1, color2);
		assertTrue(col.isValid());

		ContinuousColorRange col2 = col.clone();
		assertTrue(col2.isValid());

		col.defaultColorModel(ColorModel.Value.cmCYMKA);
		assertEquals(col.defaultColorModel(), ColorModel.Value.cmCYMKA);

		ColorDomain colDom = new ColorDomain("testdomain");
		colDom.setRange(col);
		assertEquals(colDom.containsColor(color3), "cSELF");
	}
	
	@Test
	public void colorHSLA() {
		Color color1 = new Color(ColorModel.Value.cmHSLA,
				new vectord(new double[] { 100.0, 0.2, 0.16, 1.0 }));
		Color color2 = new Color(ColorModel.Value.cmHSLA,
				new vectord(new double[] { 300.0, 0.7, 0.5, 1.0 }));
		Color color3 = new Color(ColorModel.Value.cmHSLA,
				new vectord(new double[] { 177.0, 0.5, 0.4, 1.0 }));
		
		ContinuousColorRange col = new ContinuousColorRange(color1, color2);
		assertTrue(col.isValid());

		ContinuousColorRange col2 = col.clone();
		assertTrue(col2.isValid());

		col.defaultColorModel(ColorModel.Value.cmHSLA);
		assertEquals(col.defaultColorModel(), ColorModel.Value.cmHSLA);

		ColorDomain colDom = new ColorDomain("testdomain");
		colDom.setRange(col);
		assertEquals(colDom.containsColor(color3), "cSELF");
	}
}
