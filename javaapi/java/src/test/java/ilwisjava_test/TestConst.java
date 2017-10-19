package ilwisjava_test;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.n52.ilwis.java.*;

public class TestConst {
	@BeforeClass
	public static void onceExecutedBeforeAll() {
		TestUtil.onceExecutedBeforeAll();
	}

	@Test
	public void undef() {
		assertEquals(ilwisobjects.getsUNDEF(), "?");
        assertEquals(ilwisobjects.getshUNDEF(), 32767);
        assertEquals(2147483645, ilwisobjects.getIUNDEF());
        assertEquals(1e38, ilwisobjects.getFlUNDEF(), 1e33);
        assertEquals(-1e308, ilwisobjects.getRUNDEF(), TestUtil.precision);
        assertEquals(-9223372036854775808L, ilwisobjects.getI64UNDEF());
	}

}
