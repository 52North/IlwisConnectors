/* The ILWIS SWIG interface file*/

%module(docstring="The Java API for ILWIS Objects") ilwisobjects
%feature("autodoc","1");

// Hides the multiple inheritance warning
%warnfilter(813) javaapi::ColorPalette;

%typemap(javacode) std::vector<double> "  public vectord(double[] array) {
	this();
    for ( double i : array )
      add( i );
  }";

%include "exception.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"

%begin %{

%}

%{
#include "core/kernel.h"
#include "ilwisdata.h"
#include "../../IlwisCore/core/util/range.h"
#include "itemrange.h"
#include "javaapi_error.h"
#include "javaapi_extension.h"
#include "javaapi_object.h"
#include "javaapi_ilwisoperation.h"
#include "javaapi_engine.h"
#include "javaapi_ilwisobject.h"
#include "javaapi_coordinatesystem.h"
#include "javaapi_table.h"
#include "javaapi_coverage.h"
#include "javaapi_util.h"
#include "javaapi_geometry.h"
#include "javaapi_feature.h"
#include "javaapi_featureiterator.h"
#include "javaapi_featurecoverage.h"
#include "javaapi_pixeliterator.h"
#include "javaapi_georeference.h"
#include "javaapi_rastercoverage.h"
#include "javaapi_range.h"
#include "javaapi_catalog.h"
#include "javaapi_domain.h"
#include "javaapi_datadefinition.h"
#include "javaapi_columndefinition.h"
#include "javaapi_domainitem.h"
#include "javaapi_rangeiterator.h"
#include "javaapi_vertexiterator.h"
%}

%include "javaapi_qtGNUTypedefs.h"

//catch std::exception's on all C API function calls
%exception{
    try {
        $action
    } catch (std::exception& e) {
        jenv->ThrowNew(jenv->FindClass("java/lang/Exception"),javaapi::get_err_message(e));
        //SWIG_fail;
    }
}

%rename(multiply) operator*=;
%rename(multiply) operator*;
%rename(equals) operator==;
%rename(notequal) operator!=;
%rename(toString) __str__;
%rename(increase) operator+=;
%rename(decrease) operator-=;
%rename(add) operator+;
%rename(subtract) operator-;
%rename(divide) operator/=;
%rename(divide) __itruediv__;
%rename(contains) __constains__;
%rename(isValid) __bool__; // always?
%rename(get) __getitem__;
%rename(next) __next__;
%rename(add) __add__;
%rename(contains) __contains__;
%rename(iterator) __iter__;
%rename(toDouble) __float__;
%rename(toBigInteger) __int__;
%rename(set) __set__;
%rename(set) __setitem__;


%typemap(javainterfaces) javaapi::FeatureCoverage "Iterable<Feature>";
%typemap(javainterfaces) javaapi::FeatureIterator "Iterator<Feature>";
%typemap(javaimports) javaapi::FeatureIterator "import java.util.Iterator;";
%typemap(javacode) javaapi::FeatureIterator "    public void remove() { } // Filler function"

%typemap(javainterfaces) javaapi::Geometry "Iterable<Coordinate>";
%typemap(javainterfaces) javaapi::VertexIterator "Iterator<Coordinate>";
%typemap(javaimports) javaapi::VertexIterator "import java.util.Iterator;";
%typemap(javacode) javaapi::VertexIterator "    public void remove() { } // Filler function"

%typemap(javainterfaces) javaapi::RasterCoverage "Iterable<Double>";
%typemap(javainterfaces) javaapi::PixelIterator "Iterator<Double>";
%typemap(javaimports) javaapi::PixelIterator "import java.util.Iterator;";
%typemap(javacode) javaapi::PixelIterator %{  public Double next() { return _next(); }
  public void remove() { } // Filler function
%}

%include "javaapi_extension.h"

%include "javaapi_object.h"

%ignore operator=;

%include "javaapi_ilwisoperation.h"

%include "javaapi_engine.h"

%include "javaapi_ilwisobject.h"

%include "javaapi_coordinatesystem.h"

%include "javaapi_util.h"

%template(Pixel) javaapi::PixelTemplate<qint32>;
%template(PixelD) javaapi::PixelTemplate<double>;
%template(Size) javaapi::SizeTemplate<quint32>;
%template(SizeD) javaapi::SizeTemplate<double>;
%template(Box) javaapi::BoxTemplate<Ilwis::Location<qint32, false>, javaapi::PixelTemplate<qint32>, quint32>;
%template(Envelope) javaapi::BoxTemplate<Ilwis::Coordinate, javaapi::Coordinate, double>;
//%template(NumericStatistics) javaapi::ContainerStatistics<double>;

%include "javaapi_table.h"

%include "javaapi_coverage.h"

%include "javaapi_object.h"

%include "javaapi_geometry.h"

%include "javaapi_feature.h"

%include "javaapi_featureiterator.h"

%include "javaapi_featurecoverage.h"

%include "javaapi_pixeliterator.h"

%include "javaapi_georeference.h"

%include "javaapi_rastercoverage.h"

%include "javaapi_catalog.h"

%include "javaapi_domain.h"
%typemap(javainterfaces) javaapi::NumericRange "Iterable<Double>";
%include "javaapi_range.h"

%include "javaapi_rangeiterator.h"

%typemap(javainterfaces) javaapi::RangeIterator<double, javaapi::NumericRange, double, Ilwis::NumericRange> "Iterator<Double>";
%typemap(javaimports) javaapi::RangeIterator<double, javaapi::NumericRange, double, Ilwis::NumericRange> "import java.util.Iterator;";
%typemap(javacode) javaapi::RangeIterator<double, javaapi::NumericRange, double, Ilwis::NumericRange> "  public Double next() { return current(); }";
%template(NumericRangeIterator) javaapi::RangeIterator<double, javaapi::NumericRange, double, Ilwis::NumericRange>;
//%template(ItemRangeIterator) javaapi::RangeIterator<javaapi::DomainItem, javaapi::ItemRange, Ilwis::SPDomainItem, Ilwis::ItemRange>;

%include "javaapi_datadefinition.h"

%include "javaapi_columndefinition.h"

%include "javaapi_domainitem.h"

%include "javaapi_vertexiterator.h"

//%include "javaapi_error.h"
	

namespace std {
   %template(vectori) vector<int>;
   %template(vectord) vector<double>;
   %template(vectorl) vector<quint32>;
   %template (mapsd) map<std::string, double>;
   %template (vectorvs) vector<std::vector<std::string> >;
   %template (vectors) vector<std::string>;
   %template (vectorOperation) vector<javaapi::IlwisOperation>;
};

%pragma(java) moduleimports=%{
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
%} 
%pragma(java) modulecode=%{
	public static String getsUNDEF() {
		return "?";
	}

	public static int getshUNDEF() {
		return 32767;
	}

	public static float getFlUNDEF() {
		return (float) 1e38;
	}

	private static boolean libLoaded = false;
	private static String ilwisLocation = getIlwisLocation();

	/**
	 * Use before any Ilwis fuction. Set the Ilwis folder with setIlwisLocation,
	 * if no ilwislocation.config is in the resources.
	 * 
	 * @throws FileNotFoundException
	 *             Can't find Ilwis-Objects.
	 */
	public static void initIlwisObjects() throws FileNotFoundException {
		if (!libLoaded) {
			if (ilwisLocation == null) {
				ilwisLocation = readIlwisLocation();
			}
			if (ilwisLocation == null) {
				throw new FileNotFoundException(
						"Ilwis location not set and ilwislocation.config not found or not well-formed.");
			}
			if (System.getProperty("os.name").toLowerCase().contains("win")) { // Windows
				System.load(ilwisLocation + "extensions" + File.separator
						+ "_ilwisobjects" + File.separator
						+ "_ilwisobjects0.dll");
			} else { // Linux
				System.load(ilwisLocation + "extensions/_ilwisobjects/lib_ilwisobjects.so");
			}

			ilwisobjects._initIlwisObjects(ilwisLocation);

			Runtime.getRuntime().addShutdownHook(new Thread(new Runnable() {
				public void run() {
					ilwisobjects._exitIlwisObjects();
				}
			}));
			libLoaded = true;
		}
	}

	private static String readIlwisLocation() {
		BufferedReader br = null;
		// default location
		URL input = ClassLoader.getSystemResource("ilwislocation.config");
		if (input == null)
			return null;
		try {
			String line;

			br = new BufferedReader(new InputStreamReader(input.openStream()));

			while ((line = br.readLine()) != null) {
				if (line.indexOf("=") == -1) {
					continue;
				}
				String[] lineSplit = line.split("=");
				if (lineSplit[0].equals("ilwisDir")) {
					return (lineSplit[1]);
				}
			}
		} catch (IOException e) {
		} finally {
			try {
				if (br != null)
					br.close();
			} catch (IOException ex) {
			}
		}
		return null;
	}

	public static String getIlwisLocation() {
		return ilwisLocation;
	}

	/**
	 * Set before initIlwisObjects!
	 * 
	 * @param location
	 *            Ilwis-Objects location eg.
	 *            G:/IlwisObjects/output/win32release/bin/
	 */
	public static void setIlwisLocation(String location) {
		ilwisLocation = location;
	}
  
%}



	
