package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestEngine {
	private final static String workingDir = TestUtil.workingDir;
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}	
	@Before
	public void setUp() throws Exception {
		ilwisobjects.disconnectIssueLogger();
        Engine.setWorkingCatalog(workingDir+"raster/");
        ilwisobjects.connectIssueLogger();
        
        // Set up tif rasters
        RasterCoverage rc = new RasterCoverage("small.mpl");
        rc.store("small", "GTiff", "gdal");
        rc = new RasterCoverage("small2.mpr");
        assertTrue(rc.geoReference().isValid());
        rc.store("small2", "GTiff", "gdal");
        rc = new RasterCoverage("small3.mpr");
        assertTrue(rc.geoReference().isValid());
        rc.store("small3", "GTiff", "gdal");
	}

	@Test
	public void operations() {
		vectors ops = Engine.operations();
		String[] oper = new String[] {"setvaluerange", "binarymathtable", "selection", "mastergeoreference", "binarymathfeatures",
                "binarymathraster", "selection", "iff", "stringfind", "stringsub", "stringreplace", "rastersize",
                "text2output", "coord2pixel", "coordinate", "pixel", "pixel2coord", "selection", "assignment",
                "sin", "cos", "tan", "asin", "acos", "atan", "log10", "ln", "abs", "sqrt", "ceil", "floor", "sgn",
                "cosh", "sinh", "binarylogicalraster", "iff", "rastervalue", "resample", "gridding", "script",
                "aggregateraster", "areanumbering", "cross", "linearstretch", "linearrasterfilter",
                "rankorderrasterfilter"
		};
		for(String s : oper) {
			boolean contains = false;
			for(int i=0; i<ops.capacity(); ++i) {
				if (s.equals(ops.get(i))) {
					contains = true;
					break;
				}
			}
			assertTrue(contains);
		}
		assertEquals("gridsize(rastercoverage,xsize|ysize|zsize)", Engine.operationMetaData("rastersize"));
		assertEquals(
                "gridding(coordinatesyste,top-coordinate,x-cell-size, y-cell-size, horizontal-cells, vertical-cells)",
                Engine.operationMetaData("gridding"));
		assertEquals(("iffraster(featurecoverage,outputchoicetrue, outputchoicefalse)\n"
                 + "iffraster(rastercoverage,outputchoicetrue, outputchoicefalse)"),
               Engine.operationMetaData("iff"));
	}
	
	@Test
	public void gridding() {
		CoordinateSystem cs = new CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs");
		assertTrue(cs.isValid());
		
		IObject result = Engine._do("gridding_1", "gridding", cs.toString(), new Coordinate(225358.6605, 3849480.5700).toString(),
				Double.toString(1000.0), Double.toString(1000.0), Integer.toString(12), Integer.toString(12));

		FeatureCoverage polygongrid = FeatureCoverage.toFeatureCoverage( result );
	    assertTrue(polygongrid.isValid());
	    assertEquals("wrong IlwisObject type", polygongrid.type(), "FeatureCoverage");
	    assertTrue( polygongrid.name().matches("gridding_[0-9]*") );
	    assertEquals("wrong number of polygons in gridding result!", polygongrid.featureCount(), 144);
	    
	    //polygongrid.store(workingDir + "temp/aa_gridding", "vectormap", "ilwis3");
	    polygongrid.store(workingDir + "feature/aa_gridding.shp", "ESRI Shapefile", "gdal");
	}
	
	@Test
	public void resample() { //                                    8, raster;   131072, Georef;  68719476736, String                              
		//IObject resampled = Engine._do("resample_1", "resample", "subkenya.mpr", "alm011nd.grf", "bicubic");
		IObject resampled = Engine._do("resample_1", "resample", "n000302.tif", "alm011nd.grf", "bicubic");
	    RasterCoverage resampledR = RasterCoverage.toRasterCoverage( resampled );
		assertTrue(resampledR.isValid());
		resampledR.store(workingDir + "raster/aa_resample_subkenya", "GTiff", "gdal");
		resampledR.store(workingDir + "raster/aa_resample_subkenya", "map", "ilwis3");
	}
	
	@Test
	public void resample_tif() {
		RasterCoverage rc = new RasterCoverage("n000302.tif");
		GeoReference grf = new GeoReference("alm011nd.grf");
		assertTrue(rc.isValid());
		assertTrue(grf.isValid());
		assertEquals("n000302.tif", rc.name());
		assertEquals("alm011nd.grf", grf.name());
		assertTrue(rc.isValid());
		
		IObject result = Engine._do("resample_1", "resample", rc.name(), grf.name(), "bicubic");
	    RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store(workingDir + "raster/aa_n000302", "GTiff", "gdal");
		resultR.store(workingDir + "raster/aa_n000302", "map", "ilwis3");
	}
	
	@Test // Almost fully black output
	public void linearstretch() {
		RasterCoverage rc = new RasterCoverage("n000302.tif");
		IObject result = Engine._do("lin_1", "linearstretch", rc.name(), "100", "100");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		resultR.store(workingDir + "raster/aa_linearstretch", "GTiff", "gdal");
	}
	
	@Test // Working mirrvert
		  // !! mirrhor, mirrdiag, transpose, rotate90, rotate180, rotate270 not working
	public void mirrorrotateraster() {
		IObject result = Engine._do("mirror_1", "mirrorrotateraster","n000302.tif", "mirrvert");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		assertEquals(resultR.ilwisType().intValue(), 8);
		
		resultR.store("aa_mirrorrotateraster", "GTiff", "gdal");
		resultR.store("aa_mirrorrotateraster", "BMP", "gdal");
		
		result = Engine._do("mirror_1", "mirrorrotateraster","aa_mirrorrotateraster.tif", "mirrvert");
		resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		assertEquals(resultR.ilwisType().intValue(), 8);
		
		resultR.store("aa_mirrorrotateraster2", "GTiff", "gdal");
		resultR.store("aa_mirrorrotateraster2", "BMP", "gdal");
	}
	
	@Test // Working
	public void areanumbering() {
		IObject result = Engine._do("area_1", "areanumbering","small2.tif", "4");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_areanumbering", "GTiff", "gdal");
		resultR.store("aa_areanumbering", "map", "ilwis3");
	}
	
	@Test // Working
	public void abs() {
		IObject result = Engine._do("abs_1", "abs","n000302.tif");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_abs", "GTiff", "gdal");
	}
	
	@Ignore // Fully black output
	@Test
	public void binarylogicalraster() {
		IObject result = Engine._do("binarylogicalraster_1", "binarylogicalraster","small2.tif", "small3.tif", "less");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_binarylogicalraster", "GTiff", "gdal");
	}
	
	@Ignore // EXCEPTION_ACCESS_VIOLATION in ilwiscore.dll
	@Test
	public void binarymathfeatures() {
		ilwisobjects.disconnectIssueLogger();
        Engine.setWorkingCatalog(workingDir+"feature/");
        ilwisobjects.connectIssueLogger();
        
		FeatureCoverage fc = new FeatureCoverage("rainfall_mod.shp");
		assertTrue(fc.isValid());
		
		FeatureCoverage fc2 = new FeatureCoverage("rainfall.shp");
		assertTrue(fc2.isValid());
		
		IObject result = Engine._do("bin_1", "binarymathfeatures", "rainfall.shp", "rainfall_mod.shp", "substract");
//		
//		FeatureCoverage resultF = FeatureCoverage.toFeatureCoverage(result);
//		assertTrue(resultF.isValid());
//		resultF.store(workingDir + "temp/aa_binarymathfeatures", "vectormap", "ilwis3");
	}
	
	@Test // Working
	public void binarymathraster() {
		IObject result = Engine._do("binarymathraster_1", "binarymathraster", "n000302.tif", "200", "divide");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage(result);
		assertTrue(resultR.isValid());
		resultR.store("aa_binarymathraster", "GTiff", "gdal");
	}
	
	@Test // Working
	public void binarymathraster2() {
		IObject result = Engine._do("binarymathraster_1", "binarymathraster", "small2.tif", "small3.tif", "add");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage(result);
		assertTrue(resultR.isValid());
		resultR.store("aa_binarymathraster_add", "GTiff", "gdal");
	}
	
	@Test // Working
	public void binarymathraster3() {
		IObject result = Engine._do("binarymathraster_1", "binarymathraster", "small2.tif", "small3.tif", "substract");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage(result);
		assertTrue(resultR.isValid());
		resultR.store("aa_binarymathraster_sub", "GTiff", "gdal");
	}
	
	@Test // Working
	public void cos() {
		IObject result = Engine._do("cos_1", "cos","n000302.tif");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_cos", "GTiff", "gdal");
	}
	
	@Test // Working
	public void ln() {
		IObject result = Engine._do("ln_1", "ln","n000302.tif");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_ln", "GTiff", "gdal");
	}
	
	@Ignore // couldn't handle return type (IlwisType=49152) of do(cross_1=cross(n000302.tif,n000302.tif,dontcare))
	@Test
	public void cross() {
		Engine._do("cross_1", "cross", "n000302.tif", "n000302.tif", "dontcare");
	}
	
	@Ignore // Long calculation
	@Test // Working
	public void adaptivebilateralfilter() {
		IObject result = Engine._do("adaptivebilateralfilter_1", "adaptivebilateralfilter", "n000302.tif", "31", "31", "100.0", "60.0");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_adaptivebilateralfilter", "GTiff", "gdal");
	}
	
	@Test // Working
	public void gaussianblurfilter() {
		IObject result = Engine._do("gaussianblurfilter_1", "gaussianblurfilter", "n000302.tif", "31", "31", "100.0", "60.0");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_gaussianblurfilter", "GTiff", "gdal");
	}
	
	@Ignore // wrong parameters?
	@Test
	public void iff_raster() {
		RasterCoverage rc = new RasterCoverage("small2.tif");
		assertTrue(rc.isValid());
		
		IObject result = Engine._do("iff_1", "iff", "small2.tif", "1", "200");
		// ^ java.lang.Exception: ILWIS ErrorObject: Using unitialized ilwis object
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_iff", "GTiff", "gdal");
	}
	
	@Test // Working
	public void laplacefilter() {
		IObject result = Engine._do("laplacefilter_1", "laplacefilter", "n000302.tif", "5");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_laplacefilter", "GTiff", "gdal");
	}
	
	@Ignore // Illegal quantile number value : must be between 0 and 100, found : 0
	// couldn't do(quantile_1=quantile(n000302.tif,1))
	@Test
	public void quantile() {
		IObject result = Engine._do("quantile_1", "quantile", "n000302.tif", "1");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_quantile", "GTiff", "gdal");
	}
	
	@Ignore // metadata of rankorderrasterfilter not properly initialized
	// couldn't do(rankorderrasterfilter_1=rankorderrasterfilter(n000302.tif,filter_1,120,120,60))
	@Test
	public void rankorderrasterfilter() {
		IObject result = Engine._do("rankorderrasterfilter_1", "rankorderrasterfilter", "n000302.tif", "filter_1", "120", "120", "60");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_rankorderrasterfilter", "GTiff", "gdal");
	}
	
	@Test // Working
	public void scharrfilter() {
		IObject result = Engine._do("scharrfilter_1", "scharrfilter", "n000302.tif", "1", "0");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_scharrfilter", "GTiff", "gdal");
	}
	
	@Test // Working
	public void sobelfilter() {
		IObject result = Engine._do("sobelfilter_1", "sobelfilter", "n000302.tif", "1", "0", "5");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_sobelfilter", "GTiff", "gdal");
	}
	
	@Ignore // Long calculation
	@Test // Working
	public void timesat() {
		IObject result = Engine._do("timesat_1", "timesat", "n000302.tif", "2", "true", "false", "true");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_timesat", "GTiff", "gdal");
	}
	
	@Test // Working
	public void boxfilter() {
		IObject result = Engine._do("boxfilter_1", "boxfilter", "n000302.tif", "20", "20", "true");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_boxfilter", "GTiff", "gdal");
	}
	
	@Test // Fully white output
	public void correlation() {
		IObject result = Engine._do("correlation_1", "correlation", "small2.tif", "small3.tif");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_correlation", "GTiff", "gdal");
	}
	
	@Test // Working
	public void dilatefilter() {
		IObject result = Engine._do("dilatefilter_1", "dilatefilter", "n000302.tif", "5", "ellipse", "20", "20", "0", "0");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_dilatefilter", "GTiff", "gdal");
	}
	
	@Test // Working
	public void erodefilter() {
		IObject result = Engine._do("erodefilter_1", "erodefilter", "n000302.tif", "5", "ellipse", "20", "20", "0", "0");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_erodefilter", "GTiff", "gdal");
	}
	
	@Test // Working
	public void histogramhqualization() {
		IObject result = Engine._do("histogramhqualization_1", "histogramhqualization", "n000302.tif");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_histogramhqualization", "GTiff", "gdal");
	}
	
	@Ignore // filter required
	@Test
	public void linearrasterfilter() {
		IObject result = Engine._do("linearrasterfilter_1", "linearrasterfilter", "n000302.tif", "abc.fil");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_linearrasterfilter", "GTiff", "gdal");
	}
	
	@Test // Working
	public void medianblurfilter() {
		IObject result = Engine._do("medianblurfilter_1", "medianblurfilter", "n000302.tif", "21");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_medianblurfilter", "GTiff", "gdal");
	}
	
	@Test // Working
	public void aggregateraster() {
		IObject result = Engine._do("aggregateraster_1", "aggregateraster", "n000302.tif", "Avg", "20", "true");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_aggregateraster", "GTiff", "gdal");
	}
	
	@Test // Working with mpl and multiband geotif
	public void aggregaterasterstatistics() {
		IObject result = Engine._do("aggregaterasterstatistics_1", "aggregaterasterstatistics", "small.tif", "max");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_aggregaterasterstatistics", "GTiff", "gdal");
		
		result = Engine._do("aggregaterasterstatistics_1", "aggregaterasterstatistics", "small.mpl", "max");
		resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_aggregaterasterstatistics2", "GTiff", "gdal");
	}
	
	@Test // Working
	public void relativeaggregaterasterStatistics() {
		IObject result = Engine._do("relativeaggregaterasterStatistics_1", "relativeaggregaterasterStatistics", "small.mpl", "variance", "small.mpr");
		RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		assertTrue(resultR.isValid());
		resultR.store("aa_relativeaggregaterasterStatistics", "GTiff", "gdal");
	}
	
	@Ignore // metadata of g:/ilwisobjects/output/win32release/bin/scripts/ndvi not properly initialized
	// couldn't do(script ndvi_1=G:/IlwisObjects/output/win32release/bin/scripts/NDVI(F:/documents/ilwis/ilwisObjects/IlwisJavaExtension/ilwisjava/target/test-classes/testfiles/raster/small2.mpr,F:/documents/ilwis/ilwisObjects/IlwisJavaExtension/ilwisjava/target/test-classes/testfiles/raster/small3.mpr,NDVI))
	@Test 
	public void ndvi() {
		IObject result = Engine._do("script ndvi_1",
				"G:/IlwisObjects/output/win32release/bin/scripts/NDVI",
				"F:/documents/ilwis/ilwisObjects/IlwisJavaExtension/ilwisjava/target/test-classes/testfiles/raster/small2.mpr", 
				"F:/documents/ilwis/ilwisObjects/IlwisJavaExtension/ilwisjava/target/test-classes/testfiles/raster/small3.mpr", "NDVI");
		//RasterCoverage resultR = RasterCoverage.toRasterCoverage( result );
		//assertTrue(resultR.isValid());
		//resultR.store("aa_ndvi", "GTiff", "gdal");
	}
	
}
