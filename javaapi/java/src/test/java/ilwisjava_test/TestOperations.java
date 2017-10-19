package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestOperations {
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
	public void rastersizeMetadata() {
		vectorOperation vec = Engine.operationVector("rastersize");
		assertEquals(1, vec.size());
		IlwisOperation oper = vec.get(0);
		assertEquals("rastersize", oper.getName());
		assertEquals("gridsize(rastercoverage,xsize|ysize|zsize)",
				oper.getSyntax());
		assertEquals("Raster Gridsize", oper.getLongname());
		assertEquals("?", oper.getInparameters());
		assertEquals("?", oper.getOutparameters());
		assertEquals("raster, size", oper.getKeyword());
		assertEquals(24, oper.getId());
		assertTrue(oper.isValid());

		assertEquals("rastercoverage", oper.getPinName(1)); // Starts from 1
		assertEquals("dimension", oper.getPinName(2));
		assertEquals("?", oper.getPinName(3)); // Invalid
		assertEquals(8, oper.getPinType(1));
		assertEquals(68719476736L, oper.getPinType(2));
		assertEquals(0, oper.getPinType(3)); // Invalid
		assertEquals("rastercoverage from which a dimension size is retrieved",
				oper.getPinDesc(1));
		assertEquals("?", oper.getPinDesc(2));

		assertEquals("size", oper.getPoutName(1));
		assertEquals(1073741824L, oper.getPoutType(1));
		assertEquals("Lenght in one dimension measured in grid cells",
				oper.getPoutDesc(1));

		IlwisOperation oper2 = Engine.getOperationById(24L);
		assertEquals(oper.getSyntax(), oper2.getSyntax());
	}

	@Test
	public void griddingMetadata() {
		vectorOperation vec = Engine.operationVector("gridding");
		assertEquals(1, vec.size());
		IlwisOperation oper = vec.get(0);
		assertEquals("gridding", oper.getName());
		assertEquals(
				"gridding(coordinatesyste,top-coordinate,x-cell-size, y-cell-size, horizontal-cells, vertical-cells)",
				oper.getSyntax());
		assertEquals("gridding", oper.getLongname());
		assertEquals("?", oper.getInparameters());
		assertEquals("?", oper.getOutparameters());
		assertEquals("?", oper.getKeyword());
		assertTrue(oper.getId() > 0);
		assertTrue(oper.isValid());

		assertEquals("coordinate-syste,", oper.getPinName(1));
		assertEquals("top corner", oper.getPinName(2));
		assertEquals("X cell size", oper.getPinName(3));
		assertEquals("Y cell size", oper.getPinName(4));
		assertEquals("Horizontal cells", oper.getPinName(5));
		assertEquals("Vertical cells", oper.getPinName(6));
		assertEquals(3584, oper.getPinType(1));
		assertEquals(549755813888L, oper.getPinType(2));
		assertEquals(17179869184L, oper.getPinType(3));
		assertEquals(17179869184L, oper.getPinType(4));
		assertEquals(8556380160L, oper.getPinType(5));
		assertEquals(
				"The coordinate system of the to be created polygon coverage",
				oper.getPinDesc(1));
		assertEquals(
				"The top corner of the polygonmap expressed in coordinates of the coordinate system",
				oper.getPinDesc(2));
		assertEquals(
				"Size in the x direction of a cell in the grid expressed in untis of the coordinate system",
				oper.getPinDesc(3));
		assertEquals(
				"Size in the y direction of a cell in the grid expressed in untis of the coordinate system",
				oper.getPinDesc(4));
		assertEquals("Number of cells in the x directions", oper.getPinDesc(5));

		assertEquals("output polygon coverage", oper.getPoutName(1));
		assertEquals(4L, oper.getPoutType(1));
		assertEquals("output polygon coverage", oper.getPoutDesc(1));
	}

	@Test
	public void listEverything() {
//		vectorOperation vec = Engine.operationVector("binarylogicalraster");
//		vectorOperation vec = Engine.getAllOperations();
//		System.out.println("Number of operations: " + vec.size());
//		for (int j = 0; j < vec.size(); ++j) {
//			IlwisOperation oper = vec.get(j);
//			System.out.println("Name: " + oper.getName());
//			System.out.println("Id: " + oper.getId());
//			System.out.println("Longname: " + oper.getLongname());
//			System.out.println("Syntax: " + oper.getSyntax());
//			System.out.println("Keyword: " + oper.getKeyword());
//			System.out.println("Inparameters: " + oper.getInparameters());
//			System.out.println("Outparameters: " + oper.getOutparameters());
//			for (int i = 1; oper.getPinType(i) != 0; ++i) {
//				System.out.println(i + ". pinName: " + oper.getPinName(i));
//				System.out.println(i + ". pinDesc: " + oper.getPinDesc(i));
//				System.out.println(i + ". pinType: " + oper.getPinType(i));
//			}
//			System.out.println();
//			for (int i = 1; oper.getPoutType(i) != 0; ++i) {
//				System.out.println(i + ". poutName: " + oper.getPoutName(i));
//				System.out.println(i + ". poutDesc: " + oper.getPoutDesc(i));
//				System.out.println(i + ". poutType: " + oper.getPoutType(i));
//			}
//			System.out.println();
//		}
//		
//		 vectorOperation vec = Engine.getAllOperations();
//		 for(int i=0; i<vec.size(); ++i) {
//		 System.out.print( vec.get(i).getName() + ", ");
//		 }
	}

}
