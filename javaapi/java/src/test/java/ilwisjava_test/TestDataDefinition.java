package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestDataDefinition {
	private final static String workingDir = TestUtil.workingDir;
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}
	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
		Engine.setWorkingCatalog(workingDir + "raster/");
		ilwisobjects.connectIssueLogger();
	}

	@Test
	public void datdefNumeric() {
		NumericRange numRan = new NumericRange(23.0, 3453.4, 0.2);
		NumericDomain numDom = new NumericDomain();
		numDom.setRange(numRan);
		DataDefinition datdef1 = new DataDefinition(numDom, numRan);
		DataDefinition datdef2 = new DataDefinition(numDom);

		assertEquals(datdef1.toString(), datdef2.toString());

		assertTrue(datdef1.isValid());
		assertTrue(datdef2.isValid());
		assertTrue(datdef2.range().isValid());

		assertEquals(numRan.ilwisType(), datdef1.range().ilwisType());
		assertEquals(numRan.toString(), datdef1.range().toString());
		assertEquals(numDom.toString(), datdef1.domain().toString());
		assertEquals(numRan.toString(), datdef2.range().toString());
		assertEquals(numDom.toString(), datdef2.domain().toString());

		NumericRange numRanDiff = new NumericRange(1.0, 200.0);
		NumericDomain numDomDiff = new NumericDomain();
		numDomDiff.setRange(numRan);
		DataDefinition datdef3 = new DataDefinition(numDomDiff);
		datdef2 = datdef3;
		assertEquals(datdef3.toString(), datdef2.toString());

		datdef1.range(numRanDiff);
		datdef1.domain(numDomDiff);
		assertEquals(datdef1.toString(), datdef3.toString());
	}

	@Test
	public void datdefThem() {
		ThematicRange themRan = new ThematicRange();
		themRan.add("Sea", "SE", "Area covered by the sea");
		themRan.add("Sand", "SA", "Area covered by sand");
		ItemDomain itemDom = new ItemDomain(themRan);
		DataDefinition datdef1 = new DataDefinition(itemDom, themRan);
		DataDefinition datdef2 = new DataDefinition(itemDom);

		assertEquals(datdef1.toString(), datdef2.toString());

		assertTrue(datdef1.isValid());
		assertTrue(datdef2.isValid());
		assertTrue(datdef2.range().isValid());

		assertEquals(themRan.ilwisType(), datdef1.range().ilwisType());
		assertEquals(themRan.toString(), datdef1.range().toString());
		assertEquals(itemDom.toString(), datdef1.domain().toString());
		assertEquals(themRan.toString(), datdef2.range().toString());
		assertEquals(itemDom.toString(), datdef2.domain().toString());

		ThematicRange themRanDiff = new ThematicRange();
		themRanDiff.add("Blabla", "BB", "Two Blas");
		themRanDiff.add("Blablabla", "BBB", "Three Blas");
		ItemDomain itemDomDiff = new ItemDomain(themRanDiff);
		DataDefinition datdef3 = new DataDefinition(itemDomDiff);
		datdef2 = datdef3;
		assertEquals(datdef3.toString(), datdef2.toString());

		datdef1.range(themRanDiff);
		datdef1.domain(itemDomDiff);
		assertEquals(datdef1.toString(), datdef3.toString());
	}

	@Test
	public void datdefIdent() {
		NamedItemRange identRan = new NamedItemRange();
		identRan.add("Perth");
		identRan.add("Darwin");
		ItemDomain itemDom = new ItemDomain(identRan);
		DataDefinition datdef1 = new DataDefinition(itemDom, identRan);
		DataDefinition datdef2 = new DataDefinition(itemDom);

		assertEquals(datdef1.toString(), datdef2.toString());

		assertTrue(datdef1.isValid());
		assertTrue(datdef2.isValid());
		assertTrue(datdef2.range().isValid());

		assertEquals(identRan.ilwisType(), datdef1.range().ilwisType());
		assertEquals(identRan.toString(), datdef1.range().toString());
		assertEquals(itemDom.toString(), datdef1.domain().toString());
		assertEquals(identRan.toString(), datdef2.range().toString());
		assertEquals(itemDom.toString(), datdef2.domain().toString());

		NamedItemRange identRanDiff = new NamedItemRange();
		identRanDiff.add("Berlin");
		identRanDiff.add("Hamburg");
		ItemDomain itemDomDiff = new ItemDomain(identRanDiff);
		DataDefinition datdef3 = new DataDefinition(itemDomDiff);
		datdef2 = datdef3;
		assertEquals(datdef3.toString(), datdef2.toString());

		datdef1.range(identRanDiff);
		datdef1.domain(itemDomDiff);
		assertEquals(datdef1.toString(), datdef3.toString());
	}

	@Test
	public void datdefInterval() {
		NumericItemRange interrange = new NumericItemRange();
		interrange.add("sealevel", 40.0, 100.0);
		interrange.add("dijks", 100.0, 151.0);
		ItemDomain itemDom = new ItemDomain(interrange);
		DataDefinition datdef1 = new DataDefinition(itemDom, interrange);
		DataDefinition datdef2 = new DataDefinition(itemDom);

		assertEquals(datdef1.toString(), datdef2.toString());

		assertTrue(datdef1.isValid());
		assertTrue(datdef2.isValid());
		assertTrue(datdef2.range().isValid());

		assertEquals(interrange.ilwisType(), datdef1.range().ilwisType());
		assertEquals(interrange.toString(), datdef1.range().toString());
		assertEquals(itemDom.toString(), datdef1.domain().toString());
		assertEquals(interrange.toString(), datdef2.range().toString());
		assertEquals(itemDom.toString(), datdef2.domain().toString());

		NumericItemRange interRanDiff = new NumericItemRange();
		interRanDiff.add("low", 0.0, 50.0);
		interRanDiff.add("high", 50.0, 250.0);
		ItemDomain itemDomDiff = new ItemDomain(interRanDiff);
		DataDefinition datdef3 = new DataDefinition(itemDomDiff);
		datdef2 = datdef3;
		assertEquals(datdef3.toString(), datdef2.toString());

		datdef1.range(interRanDiff);
		datdef1.domain(itemDomDiff);
		assertEquals(datdef1.toString(), datdef3.toString());
	}
	
	/*@Test
	public void datdefTime() {
		//TODO
	}*/
	
	@Test
	public void datdefColor() {
		Color color1 = new Color(ColorModel.Value.cmRGBA, new vectord(new double[] {220.0, 20.0, 30.0, 200.0}));
	    Color color2 = new Color(ColorModel.Value.cmRGBA, new vectord(new double[] {255.0, 80.0, 60.0, 240.0}));
	    ContinuousColorRange col = new ContinuousColorRange(color1, color2);
	    ColorDomain colDom = new ColorDomain();
	    colDom.setRange(col);
	    

        DataDefinition datdef1 = new DataDefinition(colDom, col);
        DataDefinition datdef2 = new DataDefinition(colDom);

        assertEquals(datdef1.toString(), datdef2.toString());

        assertTrue(datdef1.isValid());
		assertTrue(datdef2.isValid());
		assertTrue(datdef2.range().isValid());

        assertEquals(col.ilwisType(), datdef1.range().ilwisType());
        assertEquals(col.toString(), datdef1.range().toString());
        assertEquals(colDom.toString(), datdef1.domain().toString());
        assertEquals(col.toString(), datdef2.range().toString());
        assertEquals(colDom.toString(), datdef2.domain().toString());

        Color color3 = new Color(ColorModel.Value.cmRGBA, new vectord(new double[] {130.0, 0.0, 10.0, 100.0}));
        Color color4 = new Color(ColorModel.Value.cmRGBA, new vectord(new double[] {230.0, 60.0, 50.0, 240.0}));
        ContinuousColorRange colDiff = new ContinuousColorRange(color3, color4);
        ColorDomain colDomDiff = new ColorDomain();
        colDomDiff.setRange(colDiff);
        DataDefinition datdef3 = new DataDefinition(colDomDiff);
        datdef2 = datdef3;
        assertEquals(datdef3.toString(), datdef2.toString());

        datdef1.range(colDiff);
        datdef1.domain(colDomDiff);
        assertEquals(datdef1.toString(), datdef3.toString());
	}
}
