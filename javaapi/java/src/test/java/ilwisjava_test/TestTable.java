package ilwisjava_test;

import static org.junit.Assert.*;

import java.math.BigInteger;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestTable {
	private final static String workingDir = TestUtil.workingDir;
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}
	@Before
	public void setUp() {
		ilwisobjects.disconnectIssueLogger();
		Engine.setWorkingCatalog(workingDir + "feature/");
		ilwisobjects.connectIssueLogger();
	}

	@Test
	public void attributeTable() {
		Table t = new Table("rainfall.shp");
		assertEquals("rainfall.shp", t.name());
		String[] expected1 = new String[] { "RAINFALL", "JANUARY", "FEBRUARY",
				"MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER",
				"OCTOBER", "NOVEMBER", "DECEMBER", "NEWCOL", "IDENT" };
		for (int i = 0; i < expected1.length; ++i)
			assertEquals(expected1[i], t.columns().get(i));

		assertEquals(ilwisobjects.getIUNDEF(), t.columnIndex("unknownColumn"));
		assertEquals(10, t.columnIndex("OCTOBER"));

		int[] expected2 = { 48, 46, 86, 89, 44, 40, 44, 85, 89, 0, 0, 0, 0 };
		vectors vector = t.column("OCTOBER");
		for (int i = 0; i < expected2.length; ++i) {
			assertEquals(expected2[i], Integer.parseInt(vector.get(i)));
		}
		vector = t.column(10);
		for (int i = 0; i < expected2.length; ++i) {
			assertEquals(expected2[i], Integer.parseInt(vector.get(i)));
		}

		String[] tup = new String[] { "Laguna_Santa_Rosa", "175", "165", "160",
				"78", "54", "35", "16", "4", "20", "86", "173", "181", "340",
				"2" };
		vectors rec = t.record(2);
		assertEquals(rec.size(), t.columns().size());
		for (int i = 0; i < tup.length; ++i) {
			assertEquals(tup[i], rec.get(i));
		}
	}

	@Test
	public void standaloneIlwis3Table() {
		Table t = new Table("rainfall.tbt");
		assertEquals("rainfall.tbt", t.name());
		assertEquals(12, t.recordCount());
		assertEquals(14, t.columnCount());
		t.addColumn("newColumn", "value");
		assertEquals(15, t.columnCount());
		String[] expected1 = new String[] { "january", "february", "march",
				"april", "may", "june", "july", "august", "september",
				"october", "november", "december", "newcol", "ident",
				"newColumn" };
		for (int i = 0; i < expected1.length; ++i)
			assertEquals(expected1[i], t.columns().get(i));
		long[] expected2 = { 4, 5, 6 };
		for (int i = 0; i < expected2.length; ++i)
			assertEquals(expected2[i], t.select("march < 100 AND march != 87")
					.get(i));
		long[] expected3 = { 9, 10, 11 };
		for (int i = 0; i < expected3.length; ++i)
			assertEquals(expected3[i], t.select("march == ?").get(i));
		assertEquals(13, t.columnIndex("ident"));
		assertEquals(81, Integer.parseInt(t.cell("march", 4)));
		assertEquals(2, t.columnIndex("march"));

		long[] expected4 = { 81, 76, 79 };
		for (int i = 0; i < expected4.length; ++i)
			assertEquals(
					expected4[i],
					Long.parseLong(t.cell(2,
							t.select("march < 100 AND march != 87").get(i))));
		assertEquals(ilwisobjects.getRUNDEF(),
				Double.parseDouble(t.cell("newColumn", 0)), TestUtil.precision);
		t.setCell("newColumn", 0, 32);
		assertEquals(32, Long.parseLong(t.cell("newColumn", 0)));

		double[] expected5 = { 87, 87, 160, 150, 81, 76, 79, 155, 160, -1e+308,
				-1e+308, -1e+308 };
		for (int i = 0; i < expected5.length; ++i)
			assertEquals(expected5[i],
					Double.parseDouble(t.column("march").get(i)),
					TestUtil.precision);
		for (int i = 0; i < expected5.length; ++i)
			assertEquals(expected5[i], Double.parseDouble(t.column(2).get(i)),
					TestUtil.precision);
		double[] expected6 = { 175, 165, 160, 78, 54, 35, 16, 4, 20, 86, 173,
				181, 340, 2, -1e+308 };
		for (int i = 0; i < expected6.length; ++i)
			assertEquals(expected6[i], Double.parseDouble(t.record(2).get(i)),
					TestUtil.precision);
	}

	@Test
	public void standaloneGdalTable() {
		Table t = new Table("rainfall.shp");
		assertTrue(t.isValid());
		assertFalse("created a new table object with that name!!",
				t.isInternal());
		assertEquals("rainfall.shp", t.name());
		String[] expected1 = new String[] { "RAINFALL", "JANUARY", "FEBRUARY",
				"MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER",
				"OCTOBER", "NOVEMBER", "DECEMBER", "NEWCOL", "IDENT" };
		vectors actual = t.columns();
		for (int i = 0; i < expected1.length; ++i)
			assertEquals(expected1[i], actual.get(i));
		FeatureCoverage fc = new FeatureCoverage("rainfall.shp");
		assertTrue(fc.isValid());
		assertFalse("created a new table object with that name!!",
				fc.isInternal());
		String[] expected2 = new String[] { "RAINFALL", "JANUARY", "FEBRUARY",
				"MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER",
				"OCTOBER", "NOVEMBER", "DECEMBER", "NEWCOL", "IDENT" };
		for (int i = 0; i < expected2.length; ++i)
			assertEquals(expected2[i], fc.attributeTable().columns().get(i));
	}

	@Test
	public void columnDefinition() {
		FeatureCoverage fc = new FeatureCoverage("rainfall.shp");
		ColumnDefinition coldef = null, coldef2 = null;
		for (Feature feat : fc) {
			coldef = feat.attributeDefinition(1);
			coldef2 = feat.attributeDefinition("MARCH");
		}

		Table tab = fc.attributeTable();
		assertEquals(tab.columnDefinition(1).toString(), coldef.toString());
		assertEquals(tab.columnDefinition("MARCH").toString(),
				coldef2.toString());

		NumericRange numRan = new NumericRange(0.0, 500.0);
		NumericDomain numDom = new NumericDomain();
		numDom.setRange(numRan);
		DataDefinition datDef = new DataDefinition(numDom);
		ColumnDefinition colDef = new ColumnDefinition("MARCHc", datDef,
				BigInteger.valueOf(3));

		ColumnDefinition oldColDef = tab.columnDefinition(3);
		tab.setColumnDefinition(3, colDef);
		assertNotEquals(tab.columnDefinition(3).toString(), coldef2.toString());

		ColumnDefinition colDef2 = new ColumnDefinition("JANUARYc", datDef,
				BigInteger.valueOf(1));
		ColumnDefinition oldColDef2 = tab.columnDefinition("JANUARY");
		tab.setColumnDefinition("JANUARY", colDef2);
		assertNotEquals(tab.columnDefinition(1).toString(), coldef.toString());

		// restore for the other tests
		tab.setColumnDefinition(3, oldColDef);
		tab.setColumnDefinition("JANUARY", oldColDef2);
	}

	@Test
	public void newColumn() {
		Table tab = new Table("rainfall.shp");
		long before = tab.columnCount();

		TextDomain txtDom = new TextDomain();
		DataDefinition datdef = new DataDefinition(txtDom);
		ColumnDefinition coldef = new ColumnDefinition("testText", datdef, BigInteger.valueOf(tab.columnCount()));
		tab.addColumn(coldef);
		assertEquals(tab.columnCount(), before + 1);
		tab.setCell(tab.columnCount() - 1, 1, "new Cell");
		assertEquals(tab.cell(tab.columnCount() - 1, 1), "new Cell");
	}
}
